#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <csignal>
#include <sys/wait.h>
using namespace std;

int main() {
    int s_fd, c_fd;
    char buffer[BUFSIZ];

    struct sockaddr_in s_addr, c_addr;
    socklen_t c_addr_len;

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(2222);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_fd == -1) {
        cerr << "socket error" << endl;
        return 1;
    }

    if (bind(s_fd, (struct sockaddr*) & s_addr, sizeof(s_addr)) == -1) {
        cerr << "bind error" << endl;
        return 1;
    }

    if (listen(s_fd, 128) == -1) {
        cerr << "listen error" << endl;
        return 1;
    }

    c_addr_len = sizeof(c_addr);
    while (true) {
        if ((c_fd = accept(s_fd, (struct sockaddr*) & c_addr, &c_addr_len)) == -1) {
            cerr << "accept error" << endl;
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            cerr << "fork error" << endl;
        } else if (pid == 0) {
            close(s_fd);
            while (true) {
                int recv_ret = recv(c_fd, buffer, sizeof(buffer), 0);
                if (recv_ret <= 0) {
                    close(c_fd);
                    cerr << "client closed" << endl;
                    break;
                }
                for (auto& i : buffer) {
                    i = toupper(i);
                }
                send(c_fd, buffer, recv_ret, 0);
                write(STDERR_FILENO, buffer, recv_ret);
            }
            exit(0);
        } else {
            close(c_fd);
            signal(SIGCHLD, [](int signum) {
                // 处理子进程退出的信号
                int status;
                pid_t pid;
                while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
                    cerr << "Child process " << pid << " terminated" << endl;
                }
            });
        }
    }
    return 0;
}
