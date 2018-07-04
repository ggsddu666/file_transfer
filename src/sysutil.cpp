#include "sysutil.h"
#include "common.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace sysutil;

namespace sysutil {

/*
 *  主动模式向客户端发起链接
 *  @parm : port 端口号
 *  @return : 套接字
 * */
int tcp_client(unsigned short port) {
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        ERR_EXIT("socket create error");
    }
    if(port > 0) {
        // 设置套接字选项
        int on = 1;
        if((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
            ERR_EXIT("setsockopt error");
        }

        char ip[16] = {0};  // ipv4最多16位
        getlocalip(ip);

        // 构建sockaddr
        struct ::sockaddr_in localaddr;
        memset(&localaddr, 0, sizeof(localaddr));
        localaddr.sin_family = AF_INET;
        localaddr.sin_port = htons(port);
        localaddr.sin_addr.s_addr = inet_addr(ip);
        if(bind(sock, (struct sockaddr *)(&localaddr), sizeof(localaddr)) < 0)
            ERR_EXIT("bind error");

        /******** 测试代码 ********/
        /*
        if(connect(sock, (struct sockaddr*)&localaddr, sizeof(sockaddr)) < 0)
            ERR_EXIT("connect error");
        char buf[100] = "hello world", buff[100];
        send(sock, buf, sizeof(buf), 0);
        recv(sock, buff, sizeof(buff), 0);
        std::cout << "client recv:" << buff << std::endl;
        */
        /******** 测试代码end ********/
    }

    return sock;
}

/*
 *  启动tcp服务器
 *  @parm : host 服务器ip或地址， port 服务器端口号
 *  @return : 套接字
 * */
int tcp_server(const char *host, unsigned short port) {
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        ERR_EXIT("socket error");

    // 设置套接字选项
    int on = 1;
    if((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
        ERR_EXIT("setsockopt error");
    }

    // 构建sockaddr
    struct ::sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    if(host != NULL) {
        if(inet_pton(AF_INET, host, &serveraddr.sin_addr) != 1) {
           // std::cout << "pton error" << std::endl;
            // inet_pton转化失败
            struct hostent *hp;
            hp = gethostbyname(host);
            if(hp == NULL)
                ERR_EXIT("gethostbyname error");
            serveraddr.sin_addr = *(struct in_addr*)hp->h_addr;
        }
    }
    else {  // 如果host为空，addr为任意地址
        serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
    }

    // bind 和 listen
    if (bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        ERR_EXIT("bind");

    if (listen(sock, SOMAXCONN) < 0)
        ERR_EXIT("listen");

    return sock;
}

/*用gethostname返回的是127.0.0.1，先硬编码好了
*/
int getlocalip(char *ip)
{
    strcpy(ip,"127.0.0.1");
    return 0;
}

/*
 *  设置I/O为非阻塞模式
 *  @parm : fd 文件描述符
 *  @return : void
 * */
void active_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if(flags == -1)
        ERR_EXIT("flags get error");

    flags |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) == -1)
        ERR_EXIT("flags set error");
}

/*
 *  设置I/O为阻塞模式
 *  @parm : fd 文件描述符
 *  @return : void
 * */
void active_block(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if(flags == -1)
        ERR_EXIT("flags get error");

    flags &= ~O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) == -1)
        ERR_EXIT("flags set error");
}

/*
 *  读超时检测
 *  @parm : fd 文件描述符
 *  @return : void
 * */



}