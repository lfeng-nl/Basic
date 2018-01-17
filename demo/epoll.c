#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define SERV_PORT 8000
#define LISTENQ 20
#define EVENT_MAX 20
#define MAXLINE 10
int main()
{
    struct epoll_event ev, events[EVENT_MAX];
    int epfd;
    int listenfd;
    int conn_fd;
    int nfds;
    struct sockaddr_in clientaddr, serveraddr;
    char buf[MAXLINE];
    
    epfd = epoll_create(256);
    if(epfd == -1)
    {
        printf("epoll_create error\n");
        exit(-1);
    }
                                                                                        // socket 相关函数
    listenfd = socket(AF_INET, SOCK_STREAM, 0);                                         // 创建socket
                                                    // epoll 相关函数
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);  // 将listenfd 添加到epoll
    
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serveraddr.sin_port = htons(SERV_PORT);
    bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));                  // bind命名
    listen(listenfd, LISTENQ);                                                           // 监听请求

    while(1)                                                                             // 循环提供服务
    {
        int i;
        nfds = epoll_wait(epfd, events, EVENT_MAX, -1);

        for(i=0;i<nfds;i--)                         // 处理发生事件
        {
            if(events[i].data.fd == listenfd)       // listenfd 有新事件发生  
            {
                socklen_t clilen;
                clilen = sizeof(struct sockaddr_in);
                conn_fd = accept(listenfd, (struct sockaddr *)&clientaddr, &clilen);
                printf("accept a new client: %s\n", inet_ntoa(clientaddr.sin_addr));
                ev.data.fd = conn_fd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd,EPOLL_CTL_ADD, conn_fd, &ev);
            }
            else if(events[i].events & EPOLLIN)     // 可读事件
            {   
                int sock_fd;
                if((sock_fd = events[i].data.fd) <0)
                    continue;
                int n;
                if((n = recv(sock_fd, buf, MAXLINE, 0)) < 0)
                {
                    if(errno == ECONNRESET)
                    {
                        close(sock_fd);
                        events[i].data.fd = -1;

                    }
                    else
                    {
                        printf("readline error\n");
                    }
                }
                else if(n == 0)
                {
                    close(sock_fd);
                    printf("close fd\n");
                    events[i].data.fd = -1;
                }
                
                printf("%d --> %s\n", sock_fd, buf);
                ev.data.fd = sock_fd;
                ev.events = EPOLLOUT;
                epoll_ctl(epfd, EPOLL_CTL_MOD, sock_fd, &ev);
            }

            else if(events[i].events & EPOLLOUT)    // 可写事件
            {
                int sock_fd;
                sock_fd = events[i].data.fd;
                printf("OUT\n");
                scanf("%s", buf);
                send(sock_fd, buf, MAXLINE, 0);

                ev.data.fd = sock_fd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_MOD, sock_fd, &ev);
            }
        }
    }

    return 0;
}
