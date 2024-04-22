#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>

// 回调函数，用于将接收到的数据转换为大写
void to_uppercase(struct bufferevent *bev, void *ctx) {
    char buffer[1024];
    size_t n;

    while ((n = bufferevent_read(bev, buffer, sizeof(buffer))) > 0) {
        // 将每个字符转换为大写
        for (size_t i = 0; i < n; ++i) {
            buffer[i] = toupper(buffer[i]);
        }
        // 将转换后的数据写入输出缓冲区
        bufferevent_write(bev, buffer, n);
    }
}


// 回调函数，用于处理新的连接
void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx) {
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev;

    // 获取客户端的IP地址
    struct sockaddr_in *sin = (struct sockaddr_in *)address;
    std::cout << "Client connected: " << inet_ntoa(sin->sin_addr) << "port: " << ntohs(sin->sin_port);

    // 创建一个新的bufferevent
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, to_uppercase, nullptr, nullptr, nullptr);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

int main() {
    struct event_base *base;
    struct evconnlistener *listener;
    struct sockaddr_in sin;

    // 初始化libevent
    base = event_base_new();

    // 设置监听地址和端口
    bzero(& sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8080);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

    // 创建监听器
    listener = evconnlistener_new_bind(base, accept_conn_cb, nullptr,
                                       LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                       -1, (struct sockaddr*)&sin, sizeof(sin));
    if (!listener) {
        perror("Couldn't create listener");
        return 1;
    }

    // 进入事件循环
    event_base_dispatch(base);

    // 清理资源
    evconnlistener_free(listener);
    event_base_free(base);

    return 0;
}
