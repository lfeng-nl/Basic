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
### 3.地址转换函数

- ```c
  int inet_aton(const char *strptr, struct in_addr *addrptr);   // 将点分字符串转为 in_addr
  in_addr_t 	inet_addr(const char *strptr);					  // 同上
  char *	inet_ntoa(struct in_addr inaddr);					  // 返回一个点分字符串指针；
  int inet_pton(int family, const char *strptr, void *addrptr); // 将点分字符串转为 
  const char *inet_ntop(int family, const void *addrptr, char *strptr, size_t len); // 相反
  ```

  - `family`：`AF_INET; AF_INET6`
  - ​
### 4.字节排序函数

-   大端：高尾端，尾部在高地址；

-   小端：低尾端，尾部在低地址；

-   主机序列：host byte order;

    ```c
    uint16_t htons(uint16_t host16bitvalue);
    uint32_t htons(uint32_t host32bitvalue);
    uint16_t ntohs(uint16_t net16bitvalue);
    uint32_t ntohl(uint32_t net16bitvalue);		// h表示host，n代表network，s表示short，l表示long
    ```

### 5.字符操作函数

- ```c
  void *memset(void *dest, int c, size_t len);
  void *memcpy(void *dest, const void *src, size_t nbytes);
  ```


### 6.从IPv4转到IPv6

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

    -   `domain`：协议族，`AF_INET:IPv4 Internet Protocols; AF_INET6:IPv6 Internet Protocols` ；

    >   `AF_xxx`和`PF_xxx`：可以认为相同

    -   `type` ：套接字通信特性，面向连接或无连接；`SOCK_STREAM:TCP协议； SOCK_DGRAM:UDP协议 ` ；
    -   `protocol`：用于指定协议，0，表示使用默认协议；
    -   返回`socket`的描述符；失败返回-1；

### 3.bind()

-   ```c
    int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
    ```

    -   `sockfd`：socket描述符号；
    -   `my_addr` ：用于记录地址信息；协议族、IP地址、端口号；
    -   `addrlen`：地址长度；


查看已经使用端口号：`/etc/services`

