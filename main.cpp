#include "webserv.h"

int main()
{
    int fd_server;

    // Test for responses!
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1);

    fd_server = socket(AF_INET, SOCK_STREAM, 0);
    // AF_INET for IPv4 (http), SOCK_STREAM as a standart byte based sequenced stream (for tcp), 0 as no specific protocol is used
    
    if (fd_server == -1)
    {
        std::cerr << "Socket creation failed" << std::endl;
        return (1);
    }
    
    struct sockaddr_in  address;
    
    address.sin_family = AF_INET;
    //always AF_INET (otherwise error returned)
    
    address.sin_port = htons(PORT);
    //port in network byte order (htonl to confert integer to network Byte order), PORTS under 1024 may only bind if privileged
    
    address.sin_addr.s_addr = INADDR_ANY;
    //IP host address (stored in network byte order), INADDR_ANY to assign to 0.0.0.0 = any address, use inet_aton() to convert IP Address into network byte order
    
    if (bind(fd_server, (struct sockaddr *)&address, sizeof(address)) != 0)
    {
    //bind to assign an address to the file descriptor
        std::cerr << "Binding to socket failed" << std::endl;
        close(fd_server);
        return (1);
    }

    if (listen(fd_server ,SOMAXCONN) != 0)
    {
    //marks socket as a passive socket that will be used to accept connections using accept
    //takes the fd and a int as a backlog (how many request can be queued)
        std::cerr << "Setting file descriptor to listen failed" << std::endl;
        close(fd_server);
        return (1);
    }
    std::cout << "Server is now listening on port " << PORT << std::endl;
    int addr_len = sizeof(address);
    while (true)
    {
        int fd_new_socket = accept(fd_server, (struct sockaddr *)&address, (socklen_t *)&addr_len);
        //accepting the first connection req from the queue. Creates a new connected socket (the old socket is unaffected)

        if (fd_new_socket == -1)
        {
            std::cerr << "Accept failed" << std::endl;
            close(fd_server);
            return (1);
        }

        char buffer[2048] = {0};
        read(fd_new_socket, buffer, 1024);
        //just a temporary thing, use something like gnl in the future + a parser to handle requests correctly
        std::cout << "Received request: " << std::endl << buffer << std::endl;

        if (dist(gen) == 0)
        {
            const char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\nHello World this is from Webserv!";
            send(fd_new_socket, response, strlen(response), 0);
        }
        else
        {
            const char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\nHello, World this is the other response from Webserv!";
            send(fd_new_socket, response, strlen(response), 0);
            //same as write if the FLAGS are set to 0 like rn
        }
        //basic hello world page, doesnt matter what the actuall request is for now


        close(fd_new_socket);
    }
    close(fd_server);

    return (0);
}
