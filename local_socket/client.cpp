#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <cstdio> // 添加头文件以使用 fgets

using namespace std;

int main() {
    int cfd, size;
    struct sockaddr_un servaddr, cliaddr;
    char input[BUFSIZ]; // 用于存储终端输入的缓冲区

    // 创建本地套接字
    cfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (cfd == -1) {
        perror("socket");
        return 1;
    }

    // 设置local地址结构
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sun_family = AF_UNIX;
    strcpy(cliaddr.sun_path, "/home/leyang/NetworkProjects/local_socket/client_socket");

    unlink(cliaddr.sun_path);
    socklen_t client_len = offsetof(struct sockaddr_un, sun_path) + strlen(cliaddr.sun_path) + 1;
    if (bind(cfd, (struct sockaddr*) & cliaddr, client_len) == -1 ) {
        perror("bind error");
        close(cfd);
        return 1;
    }

    // 设置服务器地址结构
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, "/home/leyang/NetworkProjects/local_socket/server_socket");

    socklen_t server_len = offsetof(struct sockaddr_un, sun_path) + strlen(servaddr.sun_path) + 1;

    // 连接到服务器
    if (connect(cfd, (struct sockaddr*) &servaddr, server_len) == -1) {
        perror("connect");
        close(cfd);
        return 1;
    }

    // 从终端读取输入并发送给服务器
    cout << "Enter message to send to server (type 'exit' to quit):" << endl;
    while (fgets(input, sizeof(input), stdin)) {
        // 如果输入了 "exit"，则退出循环
        if (strcmp(input, "exit\n") == 0) {
            break;
        }

        // 发送输入的消息给服务器
        if (send(cfd, input, strlen(input), 0) == -1) {
            perror("send");
            close(cfd);
            return 1;
        }


        // 接收服务器的响应
        size = read(cfd, input, sizeof(input));
        if (size == -1) {
            perror("read");
            close(cfd);
            return 1;
        }

        // 打印服务器响应
        write(STDOUT_FILENO, input, size);
    }

    // 关闭套接字
    close(cfd);

    return 0;
}
