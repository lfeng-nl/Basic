本文记录UNIX网络编程相关学习过程。

# 基本套接字编程

![套接字流程](https://images0.cnblogs.com/blog/349217/201312/05232335-fb19fc7527e944d4845ef40831da4ec2.png)

## 1.套接字编程简介

### 1.IPv4套接字地址结构

```c
struct in_addr{
  	in_addr_t 	saddr;				// 32-bit IPv4地址
}
struct sockaddr_in{
  	uint8_t 	sin_len;				
  	sa_family_t sin_family;			// AF_INET
  	in_port_t 	sin_port;				// 16-bit port number
  	struct 		in_addr sin_addr;		// 32-bit IPv4 address
  	char 		sin_zero[8];				// unused
}
```

-   通用套接字地址结构，ANSI C之前为了通一传递的数据类型，定义了`struct sockaddr` 通用套接字地址；函数统一使用通用套接字地址结构，做到统一；使用时通过强制类型转换传入指针，长度；从开发者角度，通用套接字地址唯一用途就是用来做强制转换；

    ```c
    struct sockaddr_in serv;								// 我们定义的 IPVC4 地址

    bind(sockfd, (struct sockaddr *)&serv, sizeof(serv));	// 强制转换传入参数
    ```

### 2.IPv6套接字地址

```c
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

### 3.字节排序函数

-   大端：高尾端，尾部在高地址；

-   小端：低尾端，尾部在低地址；

-   主机序列：host byte order;

    ```c
    uint16_t htons(uint16_t host16bitvalue);
    uint32_t htons(uint32_t host32bitvalue);
    uint16_t ntohs(uint16_t net16bitvalue);
    uint32_t ntohl(uint32_t net16bitvalue);		// h表示host，n代表network，s表示short，l表示long
    ```

### 4.字符操作函数

```c
void *memset(void *dest, int c, size_t len);
void *memcpy(void *dest, const void *src, size_t nbytes);
```

### 5.地址转换函数

```c
int 		inet_aton(const char *strptr, struct in_addr *addrptr);	// 将点分字符串转为 in_addr
in_addr_t 	inet_addr(const char *strptr);							// 
char *		inet_ntoa(struct in_addr inaddr);						// 返回一个点分字符串指针；
```

