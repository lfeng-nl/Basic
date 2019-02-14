本文记录UNIX网络编程相关学习过程。

# 基本套接字编程

![套接字流程](https://images0.cnblogs.com/blog/349217/201312/05232335-fb19fc7527e944d4845ef40831da4ec2.png)

## 1.套接字编程简介

### 1.IPv4套接字地址结构

-   ```c
    struct in_addr{
      	in_addr_t 	saddr;				// 32-bit IPv4地址,4个字节的地址
    }
    struct sockaddr_in{
      	uint8_t 	sin_len;				
      	sa_family_t sin_family;			// AF_INET
      	in_port_t 	sin_port;				// 16-bit port number
      	struct 		in_addr sin_addr;		// 32-bit IPv4 address
      	char 		sin_zero[8];				// unused
    }
    ```

>   通用套接字地址结构，ANSI C之前为了通一传递的数据类型，定义了`struct sockaddr` 通用套接字地址；函数统一使用通用套接字地址结构，做到统一；使用时通过强制类型转换传入指针，长度；
>
>   >   从开发者角度，通用套接字地址唯一用途就是用来做强制转换；

-   ```c
    struct sockaddr_in serv;								// 我们定义的 IPVC4 地址
    // 例如
    bind(sockfd, (struct sockaddr *)&serv, sizeof(serv));	// 强制转换传入参数
    ```

### 2.IPv6套接字地址

-   ```c
    struct in6_addr{
      	unit8_t 		a6_addr[16];			// 128-bit IPv6 address
    }
    struct sockaddr_in6{
      	uint8_t 		sin6_len;				// 28,
      	sa_family_t		sin6_family;			//	AF_INET6
      	in_port_t 		sin6_port;				//	poet number
      	uint32_t 		sin6_flowinfo;			//	flow information 
      	struct in6_addr sin_addr;				//	IPV6 address
      	uint32_t 		sin6_scope_id;			// 
    }
    ```
### 3.sockaddr_storage

```c
struct sockaddr_storage {
    u_char sa_len;
    u_char sa_family;
    u_char padding[128];
}; 
```
### 4.地址转换函数
- ```c
  int inet_aton(const char *strptr, struct in_addr *addrptr);   // 将点分字符串转为 in_addr
  in_addr_t 	inet_addr(const char *strptr);					  // 同上
  char *	inet_ntoa(struct in_addr inaddr);					  // 返回一个点分字符串指针；
  int inet_pton(int family, const char *strptr, void *addrptr); // 将点分字符串转为 
  const char *inet_ntop(int family, const void *addrptr, char *strptr, size_t len); // 相反
  ```

  - `family`：`AF_INET; AF_INET6`
  - ​
### 5.字节排序函数

-   大端：高尾端，尾部在高地址；

-   小端：低尾端，尾部在低地址；

-   主机序列：host byte order;

    ```c
    uint16_t htons(uint16_t host16bitvalue);
    uint32_t htons(uint32_t host32bitvalue);
    uint16_t ntohs(uint16_t net16bitvalue);
    uint32_t ntohl(uint32_t net16bitvalue);		// h表示host，n代表network，s表示short，l表示long
    ```

### 6.字符操作函数

- ```c
  void *memset(void *dest, int c, size_t len);
  void *memcpy(void *dest, const void *src, size_t nbytes);
  ```


### 7.从IPv4转到IPv6

-   1.尝试使用`getaddrinfo()`获取所有`struct sockaddr`信息，而不是手动打包；
-   2.编码中所有跟IP版本相关的用函数包装；
-   3.将`AF_INET --> AF_INET6`;
-   4.将`PF_INET --> PF_INET6`

>   `AF_xxx` ：表示地址族，`PF_xxx`表示协议族；

## 2.基本TCP套接字编程

###1.getaddrinfo

-   ```c
    int getaddrinfo(const char *node, const char *service, const sturct addrinfo *hints, struct addrinfo ** res);
    ```

    -   用以替代`gethostbyname()`;提供DNS和服务查询；

    -   `node` ：主机名或IP；例如`"www.baidu.com"`;

    -   `service` : 服务名或端口号；例如`“http”,"80","ftp"`;

    -   `hints` : 已经填充好了相关信息；给`getaddrinfo()`提示，

    -   `res` ：指向一个链表，该链表用于存储`getaddrinfo()`反馈的结果；

    -   ```c
        struct addrinfo {
                       int              ai_flags;
                       int              ai_family;
                       int              ai_socktype;
                       int              ai_protocol;
                       socklen_t        ai_addrlen;
                       struct sockaddr *ai_addr;
                       char            *ai_canonname;
                       struct addrinfo *ai_next;
                   };
        ```


### 2.socket()

-   ```c
    int socket(int domain, int type, int protocol);
    ```

    -   `domain`：协议族，`AF_INET:IPv4 Internet Protocols; AF_INET6:IPv6 Internet Protocols； AF_UNSPEC:任和协议，包括4和6` 

    >   `AF_xxx`和`PF_xxx`：可以认为相同

    -   `type` ：套接字通信特性，面向连接或无连接；`SOCK_STREAM:TCP协议； SOCK_DGRAM:UDP协议 ` ；
    -   `protocol`：用于指定协议，0，表示使用默认协议；
    -   返回`socket`的描述符；失败返回-1；

### 3.bind()
将套接字绑定到本机一个端口号上（服务端需要此步骤）；

-  ```c
    int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
    ```

    -   `sockfd`：socket描述符号；
    -   `my_addr` ：用于记录自己的地址信息；协议族、IP地址、端口号；一般需要强制类型转化为`(struct sockaddr *);`
    -   `addrlen`：地址长度，sizeof(my_addr)；
    -   推荐使用`getaddrinfo()`获取地址相关信息，
    -   `bind()`已经使用的地址和端口号会失败，失败返回-1；可以使用`setsockopt()`

### 4.connect()

-  ```c
    int connect(int sockfd, struct sockaddr * serv_addr, int addrlen);
    ```

    -  `sockfd` ：上面创建的`sockfd`；
    -  `serv_addr` ：是包含目标端口和目标服务器的IP地址；一般需要强制类型转换为`struct sockaddr *` ；
    -  `addrlen` ：服务器地址结构的字节长度；
    -  地址和地址长度一般可以通过`getaddrinfo()`获得；
    -  失败返回-1；

### 5.linten()
监听特定的端口；
  - ```c
       int listem(int sockfd, int backlog);
       ```
    - `backlog` ： 允许连接的数目；
    - 调用`listen()`之前，需要调用`bind()`绑定特定端口号；
    - 失败返回-1；

### 6.accept()
接受相应的连接，并返回一个全新的套接字文件描述符；
- ```c
    int accept(int sockfd, sturct sockaddr * addr, socklen_t *addrlen);
    ```
    - `sockfd`：上面创建，并监听的socket文件； 
    - `addr` ：通常是一个本地`struct sockaddr_storage`指针，用于存放客户端地址信息；
    - `addrlen` ：指向本地`socklen_t`变量，应该先置`sizeof(sockaddr_storage)`, 存放返回的地址长度;

### 7send()和recv()
用于面向连接的数据流套接字，如果使用面向非连接的数据报套接字，可以使用`sendto()与 recvfrom()` ;
- ```c
  int send(int sockfd, const void *msg, int len, int flags);
  int recv(int sockfd, void *buf, int len, int flags);
  ```
  - `sockfd` ：可以是`socket()`(客户端)返回的；或是`accept()`（服务端）返回的；
  - `msg`/ `buf`：缓冲区地址；
  - `len` ：以字节为单位的数据长度；需要发送的数据长度/可以接受的数据长度；
  - `flags` ：通常置为0；
  - 返回值：`recv()`返回接受到的字符长度，收到0意味着远程服务已经关闭了连接；-1为失败；`send()`返回实际发送的字节数，错误返回-1；

###  8.close()
将套接字标记为已关闭，此套接字不能再使用，TCP将尝试发送已排队等待发送到对端的任何数据；
### 9.其他

-   ```c
    int getpeername(int sockfd, struct sockaddr *addr, int *addrlen);
    ```

    -   查询套接字的另一端的地址信息；

-   ```c
    int gethostname(char *hostname, size_t size);
    ```

    -   查询自身主机名称；







查看已经使用端口号：`/etc/services`