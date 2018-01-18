# 本文介绍TCP/IP协议先关内容

## 1.网络基础知识

-   OSI参考模型：==物理层== -> ==数据链路层== -> ==网络层== -> ==传输层== -> ==会话层== -> ==表示层== -> ==应用层==
-   OSI协议以OSI参考模型为基础界定了每个阶层的协议和每个阶层之间接口相关的标准；
    -   7.应用层：针对特定应用，如电子邮件，远程登录，文件传输；
    -   6.表示层：设备固有数据格式与网络标准数据格式的转换；
    -   5.会话层：通信管理。负责建立和断开通信连接；
    -   4.传输层：管理两个节点之间的数据传输。负责可靠传输；
    -   3.网络层：地址管理与路由选择；
    -   2.数据链路层：互联设备之间传送和识别数据帧。
    -   1.物理层：物理接口，连接器；
-   TCP/IP 分层：==物理层==（以太网，无线LAN，PPP，）-->  ==网络层==（ARP，IPv4 ，IPVC6 ，ICMP）--> ==传输层== （TCP, UDP...） --> ==应用层==（SSH,HTTP,SMTP,POP)
-   WAN(Wide Area Nerwork, 广域网)
-   LAN（Local Area Nerwork，局域网）
-   传输方式：面向有连接与面向无连接型；
-   单播和广播：
    -   单播：就是1对1通信；
    -   广播：1对多
    -   多播：与广播类似，也是将消息发送给多个接收主机，不同之处在于多播要限定某一组主机作为接收端。虽然一对多，但是限定多是谁；例如电视会议；
    -   任播：特定的多台主机中选出一台作为接收端的一种通信方式；面向多个，但最终选择单个特定主机通信；如DNS；
-   地址：每一层协议所使用的地址不尽相同；例如MAC地址，IP地址，端口号，等；
-   网络设备：
    -   网卡：使计算机联网的设备，也叫网卡，LAN卡；
    -   网桥：从数据链路层上延长网络的设备；
    -   路由：通过网络层转发分组数据的设备；
    -   交换机器：处理传输层以上各层网络传输的设备；
-   云：利用虚拟化技术，根据需要自动进行动态管理分配计算机资源（存储，CPU，内存等）

## 2.TCP/IP 基础

-   RFC（Request For Comment）文档，记录协议规范内容；http://www.rfc-editor.org/rfc/   ftp://ftp.rfc-deitor.org/in-notes/
-   The Internet：互联网
-   TCP/IP 的分层模型：
    -   物理层和网络接口层：网卡及接口；
    -   网络层：IP，ICMP，ARP
    -   传输层：TCP，UDP
    -   应用层：HTTP(HyperText Transfer Protocol),FTP(File Transfer Prototol),SSH...
-   每个分层中，都会对所发送的数据附加一个首部，在这个首部中包含了该层必要的信息；


## 3.数据链路

数据链路层的协议定义了通过通信媒介互联的设备之间传输的规范。

-   MAC地址：用于识别数据链路中互联的节点；
-   以太网：Ethernet，
-   VPN:(虚拟专用网络)

## 4.IP协议

网络层协议，主要作用是“实现终端节点之间的通信”。

-   主机：一般将配有IP地址，但不进行路由控制的设备；
-   路由：即配有IP地址，又具有路由控制能力的设备叫做“路由器”
-   节点：路由和主机的统称；
-   ​
