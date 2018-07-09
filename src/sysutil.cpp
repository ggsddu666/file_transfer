#include "sysutil.h"
#include "common.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace sysutil;

namespace sysutil {

//void activate_nonblock(int fd);
//void deactivate_nonblock(int fd);
//int connect_timeout(int fd, struct ::sockaddr_in *addr, unsigned int wait_seconds);
//struct sockMsg {
//    int sock;
//    ::sockaddr_in addr;
//};
//void *threadFunction(void *arg)   // 测试connect_timeout
//{
//
//    std::cout << "threadFunction is running" << std::endl;
//    struct sockMsg sock_msg = *(struct sockMsg*)arg;
//    // std::cout << "arg is " << sock_msg <<  std::endl;
//    connect_timeout(sock_msg.sock, &sock_msg.addr, 6);
//}

int read_timeout(int fd, unsigned int wait_seconds);
//void *threadFunction(void *arg)   // 测试read_timeout
//{
//    std::cout << "threadFunction is running" << std::endl;
//    int sock = *(int *)arg;
//    std::cout << "arg is " << sock <<  std::endl;
//    read_timeout(sock, 5);
//}

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
//        if(bind(sock, (struct sockaddr *)(&localaddr), sizeof(localaddr)) < 0)
//            ERR_EXIT("bind error");

        /******** 测试代码 ********/
//
        if(connect(sock, (struct sockaddr*)&localaddr, sizeof(sockaddr)) < 0)
            ERR_EXIT("connect error");
        char buf[100] = "hello world", buff[100];
        send(sock, buf, sizeof(buf), 0);
//
        // 开个线程，去判断是否超时（测试read_timeout）

//        pthread_t threadID;
//        pthread_create(&threadID, NULL, threadFunction, &sock);


        // 开个线程，去判断是否超时（测试connect_timeout）
//        pthread_t threadID;
//        struct sockMsg sock_msg;
//        sock_msg.sock = sock;
//        sock_msg.addr = localaddr;
//        pthread_create(&threadID, NULL, threadFunction, &sock_msg);
//
       // std::cout << "in tcp_client function(buff:" << buff << ")" << std::endl;
        readline(sock, buff, 80);
        //recv(sock, buff, sizeof(buff), 0);
        std::cout << "client recv:" << buff << std::endl;


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
    // sleep(10);
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
void deactivate_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if(flags == -1)
        ERR_EXIT("flags get error");

    flags &= ~O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) == -1)
        ERR_EXIT("flags set error");
}

/*
 *  读超时检测函数，不含读
 *  @parm : fd 文件描述符, wait_seconds 超时时间
 *  @return : 未超时，返回0；否则(超时)返回-1，且errno = ETIMEDOUT
 * */
int read_timeout(int fd, unsigned int wait_seconds) {
    int ret = 0;
    if(wait_seconds > 0) {
        struct timeval timeout;
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        fd_set read_fdset;
        FD_ZERO(&read_fdset);
        FD_SET(fd, &read_fdset);

        do {
            ret = select(fd + 1, &read_fdset, NULL, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        if(ret == 0) {  // 超时
            ret = -1;
            errno = ETIMEDOUT;
            // std::cout << "read timeout~~~~~~~~~" << std::endl;
        } else if(ret == 1) {   // 未超时
            ret = 0;
        }

    }
    return ret;
}

/*
 *  写超时检测, 不含写
 *  @parm : fd 文件描述符， wait_time 等待时间
 *  @return: 未超时，返回0；否则(超时)返回-1，且errno = ETIMEDOUT
 * */
int write_timeout(int fd, unsigned int wait_seconds) {
    int ret = 0;
    if(wait_seconds > 0) {
        struct timeval timeout;
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        fd_set write_fdset;
        FD_ZERO(&write_fdset);
        FD_SET(fd, &write_fdset);

        do {
            ret = select(fd + 1, NULL, &write_fdset, NULL, &timeout);
        } while(ret < 0 && errno == EINTR);

        if(ret == 0) {
            // 超时
            ret = -1;
            errno = ETIMEDOUT;
        } else if(ret == 1) {
            // 未超时
            ret = 0;
        }
    }

    return ret;
}

/*
 *  accept超时函数，包含accept
 *  @parm : fd 文件描述符， addr 用来返回.为对方的地址, wait_time 等待时间(如果为0，表示正常模式)
 *  @return: 未超时，返回套接字；否则(超时)返回-1，且errno = ETIMEDOUT
 * */
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds) {
    int ret = 0;
    if(wait_seconds > 0) {
        struct timeval timeout;
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        fd_set accept_fdset;
        FD_ZERO(&accept_fdset);
        FD_SET(fd, &accept_fdset);

        do {
            ret = select(fd + 1, &accept_fdset, NULL, NULL, &timeout);
        } while(ret < 0 && errno == EINTR);

        if(ret == 0) {
            // 超时
            errno = ETIMEDOUT;
            return -1;
        } else if(ret == -1) {
            return -1;
        }
    }


    FTPD_LOG(DEBUG, "accept listened fd: %d", fd);
    if(addr != NULL) {
        socklen_t sockaddrlen = sizeof(struct ::sockaddr_in);
        ret = accept(fd, (struct sockaddr *)addr, &sockaddrlen);
    } else {
        ret = accept(fd, NULL, NULL);
    }

    return ret;
}

/**
 * connect_timeout - connect
 * @fd: 套接字
 * @addr: 要连接的对方地址
 * @wait_seconds: 等待超时秒数，如果为0表示正常模式
 * 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
 */
int connect_timeout(int fd, struct ::sockaddr_in *addr, unsigned int wait_seconds) {
    int ret;

    if(wait_seconds > 0)
        active_nonblock(fd);

    socklen_t addrlen = sizeof(struct ::sockaddr_in);
    ret = connect(fd, (struct sockaddr*)addr, addrlen);

    // EINPROGRESS表示还在连接中
    if(ret < 0 && errno == EINPROGRESS) {
        struct timeval timeout;
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        fd_set connect_fdset;
        FD_ZERO(&connect_fdset);
        FD_SET(fd, &connect_fdset);

        do {
            // 连接建立，套接字就可写 (可写的条件跟套接字的可用缓冲区有关)
            ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        if(ret == 0) {
            // 超时
            ret = -1;
            errno = ETIMEDOUT;
        } else if(ret < 0)
            return -1;
        else {
            // ret返回为1，可能有两种情况，一种是连接建立成功，一种是套接字产生错误
            // 此时错误信息不会保存至errno变量中，因此，需要调用getsockopt来获取。
            int err;
            socklen_t socklen = sizeof(err);
            // 第三个参数optname
            int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
            if(sockoptret == -1) {
                return -1;
            }

            if(err == 0) {
                // 没有错误，说明连接成功
                ret = 0;
            } else {
                errno = err;
                ret = -1;
            }
        }
    }


    if(wait_seconds > 0)
        deactivate_nonblock(ret);

    return ret;
}

/**
 * readn - 读取固定字节数
 * @fd: 文件描述符
 * @buf: 接收缓冲区
 * @count: 要读取的字节数
 * 成功返回count，失败返回-1，读到EOF返回<count
 */
ssize_t readn(int fd, void *buf, size_t count) {
     size_t nleft = count;
     ssize_t n;
     char *bufp = (char *)buf;
    // std::cout << "in readn function(buff:" << (char *)buf << ")"  << std::endl
            // std::cout << "in readn function(count:" << count << ")" << std::endl;

     while (nleft > 0) {
         if((n = read(fd, bufp, nleft)) < 0) {
             if(errno == EINTR) {
                 continue;
             }

             return -1;
         }
         else if(n == 0) {
             // 全部读完
             return count - nleft;
         }

         nleft -= n;
         bufp += n;
     }
//     std::cout << "in readn function(count:" << count << ")" << std::endl;
//     std::cout << "in readn function(buff:" << (char *)buf << ")"  << std::endl;
     return count;
 }

/**
* writen - 发送固定字节数
* @fd: 文件描述符
* @buf: 发送缓冲区
* @count: 要读取的字节数
* 成功返回count，失败返回-1
*/
ssize_t writen(int fd, const void *buf, size_t count) {
    size_t nleft = count;
    ssize_t nwrite;
    char *bufp = (char *)buf;

    while (nleft > 0) {
        if((nwrite = write(fd, bufp, nleft)) < 0) {
            if(errno == EINTR) {
                continue;
            }
            return -1;
        } else if(nwrite == 0) {
            continue;
        }

        bufp += nwrite;
        nleft -= nwrite;
    }

    return count;
}

/**
 * recv_peek - 仅仅查看套接字缓冲区数据，但不移除数据
 * @sockfd: 套接字
 * @buf: 接收缓冲区
 * @len: 长度
 * 成功返回>=0，失败返回-1
 */
ssize_t recv_peek(int sockfd, void *buf, size_t len) {
    //MSG_PEEK 查看当前数据,数据将被复制到缓冲区中，但并不从输入队列中删除
   while (1) {
       int ret = recv(sockfd, buf, len, MSG_PEEK);
       if(ret == -1 && errno == EINTR)
           continue;
       // std::cout << "function recv_peek,ret:" <<  ret<< std::endl;
       return ret;
   }
}

/**
 * readline - 按行读取数据
 * 阻塞模式，若返回0表示连接断开
 * @sockfd: 套接字
 * @buf: 接收缓冲区
 * @maxline: 每行包含'\n'的最大长度
 * 成功返回>=0，失败返回-1
 */
ssize_t readline(int sockfd, void *buf, size_t maxline) {
    int ret;
    char buff[maxline];
    char *bufp = (char *)buf, *buffp = buff;
    int nleft = maxline, nread;
    while (1) {
        ret = recv_peek(sockfd, buffp, nleft);
        if(ret < 0)
            return ret;
        else if(ret == 0)
            return 0;

        nread = ret;
        // 判断这nread个字符中有没有换行符
        for(int i = 0; i < ret; ++i) {
            if(buffp[i] == '\n') {
                // std::cout << "in readline function(before buf:" << (char *)buf << ")" << std::endl;
                // 如果有，只读换行符前这一段（包括换行符)
                ret = readn(sockfd, bufp, i + 1);
                // 将换行符替换为\0;
                *(bufp + i) = '\0';
                if(ret != i+1)
                    exit(EXIT_FAILURE);
                return ret;
            }
        }

        // 如果没有，读完整个nread，再循环继续读
        if(nread > nleft)
            exit(EXIT_FAILURE);
        nleft -= nread;
        ret = readn(sockfd, bufp, nread);
        if (ret != nread)
            exit(EXIT_FAILURE);
        bufp += nread;
    }

    return -1;
}

/**
 * 用unix域套接字传送文件描述符
 * @param sock_fd  unix域套接字
 * @param fd      待发送的文件描述符
 */
void send_fd(int sock_fd, int fd) {
    struct msghdr msg;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    // 构建msg_iov（放置普通数据）
    struct iovec iov;
    char sendchar = 0;
    iov.iov_base = &sendchar;
    iov.iov_len = sizeof(sendchar);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // 构建msg_control（放置辅助数据）
    struct cmsghdr *p_cmsg;
    char cmsgbuf[CMSG_SPACE(sizeof(fd))];
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);
    // 将fd放入辅助数据中
    p_cmsg = CMSG_FIRSTHDR(&msg);
    p_cmsg->cmsg_level = SOL_SOCKET;
    p_cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
    p_cmsg->cmsg_type = SCM_RIGHTS;
    int *p_fds = (int *)CMSG_DATA(p_cmsg);
    *p_fds = fd;

    int ret = sendmsg(sock_fd, &msg, 0);
    if(ret != 1) {
        ERR_EXIT("sendmsg");
    }
}
}