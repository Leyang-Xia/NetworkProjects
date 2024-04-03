#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
using namespace std;

int main() {
    char buffer[BUFSIZ];
    struct sockaddr_in s_addr, c_addr;
    bzero(& s_addr, sizeof(s_addr));
    bzero(& c_addr, sizeof(c_addr));

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(8888);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int s_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (bind(s_fd, (struct sockaddr*) & s_addr, sizeof(s_addr)) == -1 ) {
        cerr << "bind error" << endl;
        exit(1);
    }

    socklen_t c_addr_len = sizeof(c_addr);
    while (1) {
        int recv_ret = recvfrom(s_fd, buffer, BUFSIZ, 0, (struct sockaddr*) & c_addr, & c_addr_len);
        if (recv_ret == -1) {
            cerr << "recvfrom error" << endl;
            exit(1);
        }

        cout << "Receive from: " << inet_ntoa(c_addr.sin_addr) << " at port: " << htons(c_addr.sin_port) << endl;

        for (int i = 0; i < recv_ret; i++) {
            buffer[i] = toupper(buffer[i]);
        }

        int send_ret = sendto(s_fd, buffer, recv_ret, 0, (struct sockaddr*) & c_addr, sizeof(c_addr));
        if (send_ret == -1) {
            cerr << "sendto error" << endl;
        }
    }
}