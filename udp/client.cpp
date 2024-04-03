#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // 准备服务端地址结构体
    struct sockaddr_in servaddr;
    std::memset(&servaddr, 0, sizeof(servaddr)); // 初始化为0
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_port = htons(8888); // 端口号8888
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr); // IP地址

    // 绑定套接字 useless in udp
//    if (bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1) {
//        std::cerr << "Error binding socket" << std::endl;
//        close(sockfd);
//        return 1;
//    }

    char buffer[BUFSIZ];
    while (fgets(buffer, BUFSIZ, stdin) != nullptr) {
        // 发送数据到服务端
        ssize_t n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        if (n == -1) {
            std::cerr << "sendto error" << std::endl;
            continue;
        }

        // 从服务端接收数据
        n = recvfrom(sockfd, buffer, BUFSIZ, 0, nullptr, nullptr); // nullptr表示不关心对端信息
        if (n == -1) {
            std::cerr << "recvfrom error" << std::endl;
            continue;
        }

        // 将接收到的数据写入标准输出
        write(STDOUT_FILENO, buffer, n);
    }

    // 关闭套接字
    close(sockfd);
    return 0;
}
