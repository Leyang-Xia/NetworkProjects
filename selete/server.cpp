#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <set>

using namespace std;

#define SERVE_PORT 8888
#define MAX_CLIENTS 1024 // 最大客户端连接数

int main() {
    int lfd, cfd, ret, len, j;
    char buf[BUFSIZ];

    struct sockaddr_in serve_addr, client_addr;
    socklen_t client_addr_len;

    bzero(&serve_addr, sizeof(serve_addr)); // 初始化地址结构体

    serve_addr.sin_family = AF_INET;
    serve_addr.sin_port = htons(SERVE_PORT);
    serve_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 创建socket
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        cerr << "socket error: " << strerror(errno) << endl;
        exit(1);
    }

    // 绑定ip和端口
    if (bind(lfd, (struct sockaddr *)&serve_addr, sizeof(serve_addr)) == -1) {
        cerr << "bind error: " << strerror(errno) << endl;
        exit(1);
    }

    // 设置上限
    if (listen(lfd, MAX_CLIENTS) == -1) {
        cerr << "listen error: " << strerror(errno) << endl;
        exit(1);
    }

    // 定义文件描述符集合
    fd_set rset, allset;
    int maxfd = lfd;
    set<int> clients; // 使用 set 来管理客户端连接

    FD_ZERO(&allset); // 清空集合
    FD_SET(lfd, &allset); // 将lfd加入到监听集合

    while (1) {
        rset = allset;

        // 设置超时时间
        struct timeval timeout;
        timeout.tv_sec = 5; // 设置超时时间为5秒
        timeout.tv_usec = 0;

        // 使用select等待文件描述符状态变化
        ret = select(maxfd + 1, &rset, nullptr, nullptr, &timeout);
        if (ret == 0) {
            cout << "select timeout" << endl;
            continue; // 如果超时，继续等待下一次事件
        } else if (ret < 0) {
            cerr << "select error: " << strerror(errno) << endl;
            exit(1);
        }

        if (FD_ISSET(lfd, &rset)) {
            // 如果lfd在传出的rset中，表示有客户端要进行连接
            client_addr_len = sizeof(client_addr);
            cfd = accept(lfd, (struct sockaddr *)&client_addr, &client_addr_len);
            if (cfd == -1) {
                cerr << "accept error: " << strerror(errno) << endl;
                exit(1);
            }

            // 显示连接方的IP地址和端口号
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
            cout << "Client connected: " << client_ip << ":" << ntohs(client_addr.sin_port) << endl;

            FD_SET(cfd, &allset); // 将cfd加入到监听集合
            if (maxfd < cfd)
                maxfd = cfd; // 更新最大的文件描述符

            if (clients.size() < MAX_CLIENTS) {
                clients.insert(cfd); // 使用 set 插入新的客户端连接描述符
            } else {
                cerr << "too many clients\n" << endl;
                exit(1);
            }

            if (ret == 1)
                continue; // select只返回了lfd这一个，后续指令无需执行，跳出本次循环，继续while
        }

        // 如果ret!=1，说明还监听到了其他描述符的read事件
        for (auto it = clients.begin(); it != clients.end(); ) {
            int fd = *it;
            if (FD_ISSET(fd, &rset)) { // 找到满足读事件的那个描述符
                len = read(fd, buf, sizeof(buf));
                if (len == 0) {
                    // 检测到客户端关闭了连接
                    close(fd); // 关闭该描述符
                    FD_CLR(fd, &allset); // 将该描述符从监听集合中移除
                    it = clients.erase(it); // 从 set 中移除该描述符
                    cout << "Client disconnected: " << fd << endl;
                } else {
                    // 如果len不为0，表示有数据，循环更改数据
                    for (j = 0; j < len; j++)
                        buf[j] = toupper(buf[j]);
                    // 写回更改后的数据
                    write(fd, buf, len);
                    write(STDOUT_FILENO, buf, len);
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }

    close(lfd);
    return 0;
}
