#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;


int main() {
    int server_fd, connect_fd;
    char buffer[BUFSIZ], client_IP[1024];
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;


    if (server_fd == -1) {
        cerr << "socket error" << endl;
    }


    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);


    int bind_ret = bind(server_fd, (struct sockaddr*) & server_addr, sizeof(server_addr));
    if (bind_ret == -1) {
        cerr << "bind error" << endl;
    }

    int listen_ret = listen(server_fd, 128);
    if (listen_ret == -1) {
        cerr << "listen error" << endl;
    }

    client_addr_len = sizeof(client_addr);
    connect_fd = accept(server_fd, (struct sockaddr*) & client_addr, & client_addr_len);
    if (connect_fd == -1) {
        cerr << "accept error" << endl;
    }


    cout << "client IP: " <<  inet_ntop(AF_INET, & client_addr.sin_addr.s_addr, client_IP, sizeof(client_IP)) << " client port: " << ntohs(client_addr.sin_port) << endl;
    while(1) {
        int read_ret = read(connect_fd, buffer, sizeof(buffer));
        write(STDOUT_FILENO, buffer, read_ret);

        for (int i = 0; i < read_ret; i++) {
            buffer[i] = toupper(buffer[i]);
        }

        write(connect_fd, buffer, read_ret);
    }

    close(server_fd);
    close(connect_fd);
    return 0;
}