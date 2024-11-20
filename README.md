# <p align="center">**Webserv**</p>
# <p align="center"> <sup>A lightweight HTTP Server in </sup> [![My Skills](https://skillicons.dev/icons?i=cpp)](https://skillicons.dev)<sup>

>## <p align="center"> <sup>Team project created by: [Eugen](https://github.com/itseugen),  [Mary Kate](https://github.com/MaryKateEvan) & [Dustin](https://github.com/StaubMaster)  </sup></p>

## 📝 Description
Webserv is our custom implementation of an **HTTP/1.1** server written in **C++17**. Designed for efficiency and compliance with HTTP standards, our server supports **non-blocking I/O operations** and is compatible with modern web browsers. It offers essential functionality for hosting static and dynamic content while ensuring scalability and flexibility.

## ✨ Features

### 💡 Core Features:
- **Non-blocking I/O:** Implements efficient I/O operations using `poll()` for all client-server communication.
- **Multi-port Support:** Ability to listen on multiple ports simultaneously.
- **HTTP Methods Supported:**
  - `GET`: Retrieve resources.
  - `POST`: Upload data to the server.
  - `DELETE`: Remove server-side resources.
- **Default Error Pages:** Provides default error pages for HTTP errors if custom ones aren't configured.
- **Stress-Test Ready:** Handles multiple simultaneous connections reliably without downtime.

### 🛠️ Configuration Options:
- **Server Setup:**
  - Server name(s) defined (e.g., `server_name`, `localhost`).
  - Listening ports and hostnames configured.
  - Default servers assigned for each `host:port`.
- **Request Handling:**
  - Limit of client body size for uploads.
  - HTTP method restrictions for specific routes.
  - Directory listing enabled or disabled.
  - Default files set for directory responses.
  - HTTP redirections per route.
- **Static and Dynamic Content:**
  - Static files are served from defined directories.
  - Execution of CGI scripts (`.py` in our case) with environment variable support.

### 🌟 Additional Features:
- **File Uploads:** Clients can upload files to designated directories.
- **CGI Execution:**
  - Supports execution of scripts based on file extensions.
  - Automatically handles chunked request decoding.
  - Manages CGI output correctly, even without content-length headers.
- **Stress Testing:** Designed to remain operational under heavy load and large numbers of simultaneous requests.

---

## 🎯 Requirements

- **C++17 Standard:** In our School we are limited to use everything up to C++17.
- **Non-blocking Design:** Ensures all I/O operations go through the `poll()` .
- **Configuration File:** The server accepts a configuration file as an argument or uses a default path if none is provided.

###   &nbsp;  &nbsp; 🚫 Forbidden Actions:
- Blocking operations.
- Using `execve` to invoke other web servers.
- Reading or writing without using `poll()` first.
- Checking `errno` after `read` or `write`.

---

## ⚙️ Installation
1. Clone the repository:
```bash
git clone https://github.com/MaryKateEvan/Webserv.git
```
2. Navigate to the project's directory:
```bash
cd Webserv
```
3. Build the program:
```bash
make
```
4. Run the Web-Server with the config file of your choice:
```bash
./webserv config_files/<file_of_your_choice>
```
&nbsp; &nbsp; &nbsp; 💡 You can also run just `./webserv`: in this case, the server uses a [default configuration](https://github.com/MaryKateEvan/Webserv/blob/main/config_files/default.conf):

---

## 🚀 Usage

You can access any of the configured servers  in a web-browser by typing in the search-bar:<br>
🔎 &nbsp;  ***localhost:<port_it_listens_to>***

 For example, if you ran `./webserv` which uses the [`default.conf`](https://github.com/MaryKateEvan/Webserv/blob/main/config_files/default.conf), there are 3 servers configured there, that listen to the ports 8080, 8081, and 8082, respectively. So you could access them on the web-broswer by typing:<br>
 - ***localhost:8080***, or <br>
 - ***localhost:8081***, or <br>
 - ***localhost:8082***

 And according to what server you are accessing, you can browse/do different things there, depending on their configuration. For example in the [default.conf](https://github.com/MaryKateEvan/Webserv/blob/main/config_files/default.conf):
 - `server1`, which listens to port `8080`, allows all methods, redirections (e.g. **localhost:8080/google**), and has a big body size, so there you can upload even large files.
 - `server2`, which listens to port `8081`, allows all methods and redirections, but you can only upload small images up to 200KB and doesn't allow directory listing.
 - `server3`, which listens to port `8082`, allows all methods and has a big body size, but it doesn't redirect because the GET method is not configured in its redirections.

## 🌱 Future Enhancements

- **Cookie and Session Management**: Support for client state management.
- **Advanced CGI Handling**: Simultaneous support for multiple CGI environments.
- **HTTPS Support**: Implementation of SSL/TLS for secure communication.

## 📜 License
This project is released under the [MIT License](https://github.com/MaryKateEvan/Webserv/blob/main/LICENSE). Contributions are welcome!