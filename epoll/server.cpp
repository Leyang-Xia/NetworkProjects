// server.cpp

// Including necessary header files
#include <iostream>     // For standard input and output
#include <string>       // For string manipulation
#include <cstring>      // For C-style string manipulation functions
#include <sys/socket.h> // For socket programming
#include <netinet/in.h> // For internet address structures
#include <unistd.h>     // For POSIX operating system API
#include <arpa/inet.h>  // For manipulating internet addresses
#include <sys/epoll.h>  // For epoll API for event notification
#include <vector>       // For using vectors
#include <algorithm>    // For algorithms like std::remove

// Maximum number of connections allowe
const int MAX_CONNECTIONS = 10;
// Server port
const int SERVER_PORT = 8888;
// Buffer size for sending and receiving data
const int BUFFER_SIZE = 1024;

int main() {
    // Creating a socket for the server
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation error" << std::endl;
        return 1;
    }

    // Setting up server address structure
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Binding address and port to the socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind error" << std::endl;
        return 1;
    }

    // Listening for incoming connections
    if (listen(server_fd, MAX_CONNECTIONS) == -1) {
        std::cerr << "Listen error" << std::endl;
        return 1;
    }

    std::cout << "Server started, waiting for connections..." << std::endl;

    // Creating an epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        std::cerr << "Epoll creation error" << std::endl;
        return 1;
    }

    // Adding listening socket to the epoll instance
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        std::cerr << "Epoll control error" << std::endl;
        return 1;
    }

    // Vector to store client file descriptors
    std::vector<int> client_fds;

    struct epoll_event events[MAX_CONNECTIONS];
    while (true) {
        // Waiting for events to occur
        int num_events = epoll_wait(epoll_fd, events, MAX_CONNECTIONS, -1);
        if (num_events == -1) {
            std::cerr << "Epoll wait error" << std::endl;
            return 1;
        }

        // Handling events
        for (int i = 0; i < num_events; ++i) {
            int fd = events[i].data.fd;

            // New connection
            if (fd == server_fd) {
                struct sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr,  &addr_len);
                if (client_fd == -1) {
                    std::cerr << "Accept error" << std::endl;
                    continue;
                }
                std::cout << "Client connected: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;

                // Adding client socket to epoll instance
                event.events = EPOLLIN;
                event.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                    std::cerr << "Epoll control error" << std::endl;
                    return 1;
                }

                client_fds.push_back(client_fd);
            } else { // Existing client sent a message
                char buffer[BUFFER_SIZE];
                int num_bytes = recv(fd, buffer, BUFFER_SIZE, 0);
                if (num_bytes <= 0) {
                    // Connection closed
                    std::cout << "Client disconnected" << std::endl;
                    close(fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                    client_fds.erase(std::remove(client_fds.begin(), client_fds.end(), fd), client_fds.end());
                } else {
                    buffer[num_bytes] = '\0';
                    std::cout << "Received message from client " << fd << ": " << buffer << std::endl;

                    // Broadcasting message to all connected clients except the sender
                    for (int client_fd : client_fds) {
                        if (client_fd != fd) {
                            send(client_fd, buffer, num_bytes, 0);
                        }
                    }
                }
            }
        }
    }

    // Closing listening socket and epoll instance
    close(server_fd);
    close(epoll_fd);

    return 0;
}
