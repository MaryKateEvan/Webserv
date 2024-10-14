#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#define PORT 8080
#define MAX_CLIENTS 42

/**
 * The poll (or select() or epoll()), is used for multiplexing
 * 
 * The usbject wants our poll() to handle both reading and writing to file descriptors (sockets)
 * SIMULTANEOUSLY. So, we must monitor each socket for both incoming data (readable) and 
 * whether it is ready to send data (writable). This prevents our program from blocking, if 
 * for example it can read data form the client but is not yet ready to write a response. 
 */

	// i must never do a read or write operation without going through poll() first
	// this means i must use poll() to check if a file descriptor is ready for reading 
	// or writing before performing the actual read ot write operation.
	// if i would try to read or write to a file descriptor that is not ready, it could
	// cause the server to BLOCK! 
	// Using POLL means we only perform these operations when they are safe and non-blocking.


int main() {

	// 1. create a TCP server socket using IPv4:
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	// AF_INET specifies that the socket will use the IPv4 protocol
	// SOCK_STREAM: specifies that the socket will be of type TCP, which provides a reliable, connection-oriented byte stream.
	// 0 for automatic selection of protocol, which will be IPPROTO_TCP
	if (server_fd < 0) {
		std::cerr << "Socket creation error\n";
		return -1;
	}

	//set up the server address struct (i am in the no configFile scenario)
	struct sockaddr_in address; // this struct is necessary when binding the socket to an IP address and port
	address.sin_family = AF_INET; //means that im working with IPv4 addresses
	address.sin_addr.s_addr = INADDR_ANY; //listen in all available interfaces: localhost, ethernet etc...
	address.sin_port = htons(PORT); //specifies the port number

	//and then i bind the socket to the specified address and port:
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		std::cerr << "Bind failed\n";
		close(server_fd);
		return -1;
	}

	// Listen for incoming connections:
	// 3 for the `backlog` which is the maximum length of the queue of pending connections
	// if more requests come in than 3 while the server is still handling the previous connections, 
	// the excess requests will be rejected or ignored by the server:
	if (listen(server_fd, 3) < 0) {
		std::cerr << "Listen failed\n";
		close(server_fd);
		return -1;
	}

	//now i prepare the pollfd structure for poll() to manage multiple clients:
	struct pollfd fds[MAX_CLIENTS];
	//only one file descriptor open now (the socket_fd) thats whay only fds[0] modified:
	fds[0].fd = server_fd;          // Monitor the server socket
	fds[0].events = POLLIN;         // We're interested in read events (POLLIN)
	// the server_socket is only waiting to receive, "read" new client connections. 
	int nfds = 1;                   // Number of active file descriptors

	std::cout << "Server listening on port " << PORT << std::endl;

	//server loop
	/*
		the server uses poll() to monitor multiple file descriptos (clients and the server socket)
	*/
	while (true) {
		int poll_count = poll(fds, nfds, -1);  // Waits indefintely for any events, on the set of file descriptors
			//poll monitors multiple sockets at once, waiting for them to become ready for some kinds of operation (reading data, writing data, error...)
			// 3rd argument is the 	timeout value`: -1 means the poll() will BLOCK INDEFINTELY until at least one
			//		file descriptor is ready (either a new connection or data to read)
		// poll_count > 0 means that one ore more fds in the fds[] array have become "ready" for an operation (like reading or accepting a connection)
		if (poll_count < 0) {
			std::cerr << "Poll error\n";
			break;
		}

		// Loop through all file descriptors
		for (int i = 0; i < nfds; i++) {
			if (fds[i].revents & POLLIN) { //check if there is data to read...
			// `revents` is set by the kernel nad indicates what kind of events actually occured in that file descriptor

				// New client connection
				if (fds[i].fd == server_fd) { // ...and if the event is on the server socket
					
					struct sockaddr_in client_addr; //struct to hold client's IP address, port, and other info
					socklen_t client_len = sizeof(client_addr); //necessary for the accept, to know how much space it has to store the lient's address
					
					//system call to accept a new connection from a client:
					int new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
					//created a file descriptor for the newly accepted connection
					if (new_socket < 0) {
						std::cerr << "Accept failed\n";
						continue; //if this connection failed, we ensure that the server doesnt try to handle a failed connection and continues monitoring other events
					}
					std::cout << "New client connected\n";

					// Add the new client socket to poll
					if (nfds < MAX_CLIENTS) {
						fds[nfds].fd = new_socket;
						fds[nfds].events = POLLIN;  // Interested in reading from client
						nfds++;
					} else {
						std::cerr << "Too many clients\n";
						close(new_socket);
					}
				}
				// Data received from a client (if the event monitored from poll is on a client's event:)
				else {
					char buffer[1024] = {0}; //buffer to store the incoming data
					//then we read the client's request from his socket:
					int valread = read(fds[i].fd, buffer, 1024); //read data from the client's socket
					if (valread <= 0) {
						std::cout << "Client disconnected\n";
						close(fds[i].fd);
						fds[i] = fds[nfds - 1];  // Remove the client from poll array
						nfds--; //decrease the number of monitored fds
					} else {
						std::cout << "Received request:\n" << buffer << "\n";

						// Send a basic HTTP response with "Hello World"
						const char *response = 
							"HTTP/1.1 200 OK\r\n"
							"Content-Type: text/html\r\n"
							"Content-Length: 23\r\n"
							"\r\n"
							"Hello World! MK here!";
						
						//send a basic HTTP response with a 200 OK status and a simple message hello world
						send(fds[i].fd, response, strlen(response), 0);
						std::cout << "Hello World sent to client " << i << std::endl;

						// Close connection after response
						close(fds[i].fd);
						fds[i] = fds[nfds - 1];
						nfds--;
					}
				}
			}
		}
	}
	close(server_fd);
	return 0;

}

/*
OVERALL, How hte above code works:
	1. Socket Creation: A socket is created using socket().
	2. Binding: The socket is bound to port 8080.
	3. Listening: The server listens for incoming connections.
	4. poll() Loop: The server uses poll() to monitor multiple file descriptors (clients and the server socket). This is non-blocking, meaning the server can handle multiple connections.
	5. Client Connection: When a new client connects, it is accepted, and the client's file descriptor is added to the poll() array.
	6. Handling Requests: When the server receives a request, it sends a simple HTTP response with "Hello World" (in my case) as the body.
	7. Connection Close: After responding, the server closes the connection to the client.

*/