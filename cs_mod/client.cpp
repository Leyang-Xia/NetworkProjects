#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;


int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFSIZ];
    int count = 10;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port  = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", & server_addr.sin_addr.s_addr);

    client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_fd == -1) {
        cerr << "socket error" << endl;
    }



    int ret = connect(client_fd, (struct sockaddr*) & server_addr, sizeof(server_addr));
    if (ret == -1) {
        cerr << "connect error" << endl;
    }

    while (count--) {
        int write_ret = write(client_fd, "hello\n", 6);
        read(client_fd, buffer, sizeof(buffer));
        write(STDOUT_FILENO, buffer, write_ret);
    }
    return 0;
}