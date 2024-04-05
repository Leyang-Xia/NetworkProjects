#include <iostream>
#include <event2/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <algorithm> // for std::transform

void read_cb(evutil_socket_t fd, short events, void *arg) {
    char buf[1024];
    auto *base = static_cast<event_base *>(arg);
    ssize_t n = recv(fd, buf, sizeof(buf), 0);
    if (n <= 0) {
        std::cerr << "Error reading from client" << std::endl;
        return;
    }
    buf[n] = '\0';

    // Convert received data to uppercase
    std::transform(buf, buf + n, buf, ::toupper);

    // Write back to client
    send(fd, buf, n, 0);
}

void accept_conn_cb(evutil_socket_t sockfd, short events, void *arg) {
    auto *base = static_cast<event_base *>(arg);

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock < 0) {
        std::cerr << "Error accepting connection from client" << std::endl;
        return;
    }

    // Convert client address to human-readable format
    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, addr_str, INET_ADDRSTRLEN);

    std::cout << "Accepted connection from " << addr_str << ":" << ntohs(client_addr.sin_port) << std::endl;

    // Create a new event for reading
    event *ev = event_new(base, client_sock, EV_READ | EV_PERSIST, read_cb, base);
    event_add(ev, nullptr);
}

int main() {
    // Initialize libevent
    event_base *base = event_base_new();
    if (!base) {
        std::cerr << "Could not initialize libevent!" << std::endl;
        return 1;
    }

    // Create a socket for listening
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Bind the socket to port 8080
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8080);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        close(sockfd);
        return 1;
    }

    // Start listening for connections
    if (listen(sockfd, 16) < 0) {
        std::cerr << "Error listening for connections" << std::endl;
        close(sockfd);
        return 1;
    }

    // Create an event for accepting connections
    event* ev_accept = event_new(base, sockfd, EV_READ | EV_PERSIST, accept_conn_cb, base);
    event_add(ev_accept, nullptr);

    std::cout << "Server started. Listening on port 8080..." << std::endl;

    // Start the event loop
    event_base_dispatch(base);

    // Clean up
    event_free(ev_accept);
    event_base_free(base);

    return 0;
}
