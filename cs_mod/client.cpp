#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>

using namespace std;


int main() {
    int fd;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port  = htons(8888);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int ret = bind(fd, (struct sockaddr*)& addr, sizeof(addr));
    return 0;
}