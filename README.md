# UDP Programming for C++
UDP 是典型的无连接协议，通信双方接收端可视为服务器，发送端可视为客户端。
接收端主要实现：使用 socket 建立套接字；通过 bind 函数把该套接字和准备接收数据的端口绑定在一起；接收数据并存入一个文件。
发送端主要实现：建立一个套接字，再调用 sendto 函数发送数据。

本项目实现的功能是：从客户端输入文件路径和文件名，然后客户端将文件传输至服务端，包括文件的创建时间、大小等信息。
包括两个文件：client.cpp和server.cpp，分别是客户端和服务端的代码。运行时需要统一客户端和服务端的IP地址及端口号。
