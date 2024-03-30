#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
using namespace std;

int main() {
    int c_fd;
    struct sockaddr_in serv_addr;
    char buffer[BUFSIZ];

    // 创建客户端套接字
    c_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (c_fd == -1) {
        cerr << "socket error" << endl;
        return 1;
    }

    // 设置服务器地址结构
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(2222);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // 连接到服务器
    if (connect(c_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        cerr << "connect error" << endl;
        return 1;
    }

    // 发送数据到服务器
    const char* message = "Hello from client";
    ssize_t sent_bytes = send(c_fd, message, strlen(message), 0);
    if (sent_bytes == -1) {
        cerr << "send error" << endl;
        return 1;
    }

    // 接收服务器的响应
    ssize_t recv_bytes = recv(c_fd, buffer, BUFSIZ, 0);
    if (recv_bytes == -1) {
        cerr << "recv error" << endl;
        return 1;
    }

    // 输出服务器响应
    cout << "Server response: " << buffer << endl;

    // 关闭套接字
    close(c_fd);

    return 0;
}
