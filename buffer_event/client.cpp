#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <unistd.h>
#include <event2/buffer.h>
#include <arpa/inet.h>
#include <cstring>

// 回调函数，用于向服务器发送数据
void send_data_cb(evutil_socket_t fd, short events, void *arg) {
    struct bufferevent *bev = (struct bufferevent *)arg;
    char data[1024];
    int nbytes;

    // 从标准输入读取数据
    nbytes = read(STDIN_FILENO, data, sizeof(data));
    if (nbytes <= 0) {
        // 读取错误或遇到EOF，停止事件循环
        event_base_loopbreak(bufferevent_get_base(bev));
        return;
    }

    // 发送数据到服务器
    bufferevent_write(bev, data, nbytes);
}

// 回调函数，用于处理从服务器接收到的数据
void read_data_cb(struct bufferevent *bev, void *ctx) {
    struct evbuffer *input = bufferevent_get_input(bev);
    char *line;
    size_t len;

    // 逐行处理接收到的数据
    while ((line = evbuffer_readln(input, &len, EVBUFFER_EOL_CRLF))) {
        printf("Received: %s\n", line);
        free(line);
    }
}

int main() {
    struct event_base *base;
    struct bufferevent *bev;
    struct sockaddr_in sin;

    // 初始化libevent
    base = event_base_new();

    // 设置服务器地址和端口
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);

    // 创建一个新的bufferevent并与服务器建立连接
    bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, read_data_cb, nullptr, nullptr, nullptr);
    if (bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bufferevent_socket_connect");
        return 1;
    }
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    // 使用标准输入事件监听键盘输入
    struct event *stdin_event = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, send_data_cb, bev);
    event_add(stdin_event, nullptr);

    // 进入事件循环
    event_base_dispatch(base);

    // 清理资源
    event_free(stdin_event);
    bufferevent_free(bev);
    event_base_free(base);

    return 0;
}
