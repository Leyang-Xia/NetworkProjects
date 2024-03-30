#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>
#include <netinet/in.h>
using namespace std;

// 处理客户端连接的线程函数
void handle_client(int c_fd, sockaddr_in& c_addr) {
    char buffer[BUFSIZ];

    // 获取客户端的 IP 地址和端口号
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(c_addr.sin_addr), ip, INET_ADDRSTRLEN);
    int port = ntohs(c_addr.sin_port);

    cout << "New connection from " << ip << ":" << port << endl;

    while (true) {
        ssize_t read_ret = recv(c_fd, buffer, sizeof(buffer), 0);
        if (read_ret <= 0) {
            close(c_fd);
            cerr << "Client closed: " << ip << ":" << port << endl;
            break;
        }

        write(STDOUT_FILENO, buffer, read_ret);

        // 处理接收到的数据
        for (int i = 0; i < read_ret; ++i) {
            buffer[i] = toupper(buffer[i]);
        }
        ssize_t send_ret = send(c_fd, buffer, read_ret, 0);
        if (send_ret == -1) {
            cerr << "send error" << endl;
            break;
        }
    }

    // 线程结束时关闭连接
    close(c_fd);
}

int main() {
    int s_fd, c_fd;
    struct sockaddr_in s_addr, c_addr;
    socklen_t c_addr_len;

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(8888);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_fd == -1) {
        cerr << "socket error" << endl;
    }

    if (bind(s_fd, (struct sockaddr*) & s_addr, sizeof(s_addr)) == -1) {
        cerr << "bind error" << endl;
    }

    if (listen(s_fd, 128) == -1) {
        cerr << "listen error" << endl;
    }

    while (true) {
        c_addr_len = sizeof(c_addr);
        c_fd = accept(s_fd, (struct sockaddr*) & c_addr, &c_addr_len);
        if (c_fd == -1) {
            cerr << "accept error" << endl;
            continue;
        }

        // 创建新的线程处理客户端连接
        thread t(handle_client, c_fd, std::ref(c_addr));
        t.detach(); // 分离线程，使得线程可以独立运行
    }

    return 0;
}
