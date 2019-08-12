/*
服务端程序
接收从客户端发送的文件，包含管理元数据（文件名、大小和日期）
*/
#include<iostream>
#include<WinSock2.h>
#include<winsock.h>
#pragma comment(lib,"ws2_32.lib")
#define BUF_SIZE 1024
#define PATH_LENGTH 20
using namespace std;
char sendBuff[BUF_SIZE];
char recvBuff[BUF_SIZE];
char fileName[PATH_LENGTH];

int main() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout << "Initialization failed." << endl;
		return -1;
	}
	SOCKET server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server == -1) {
		cout << "Socket failed." << endl;
		return -1;
	}
	sockaddr_in my_addr, remote_addr;
	int nAddrlen = sizeof(remote_addr);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(5000);
	my_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (::bind(server, (sockaddr*)&my_addr, sizeof(my_addr)) == SOCKET_ERROR) {
		cout << "Bind error!" << endl;
		return -1;
	}
	while (true) {
		cout << "---------------------RECEIVING...---------------------" << endl;
		//接收文件名
		int ret = recvfrom(server, fileName, BUF_SIZE, 0, (sockaddr*)&remote_addr, &nAddrlen);
		cout << "Filename: " << fileName << endl;
		errno_t err;
		FILE *fp;
		if ((err = fopen_s(&fp, fileName, "wb")) < 0) {
			cout << "Create failed." << endl;
			return -1;
		}
		int length;
		while ((length = recvfrom(server, recvBuff, BUF_SIZE, 0, (sockaddr*)&remote_addr, &nAddrlen))) {
			if (!strcmp(recvBuff, "end"))//接收结束信息
				break;
			if (length == 0) {
				cout << "An error occurred while receiving." << endl;
				return -1;
			}
			int ret = fwrite(recvBuff, 1, length, fp);
			if (ret < length) {
				cout << "Write failed." << endl;
				return -1;
			}
			sendto(server, "success", sizeof("success") + 1, 0, (SOCKADDR*)&remote_addr, sizeof(remote_addr));
		}
		//接收文件创建日期、大小信息
		char creationTime[30];
		char fileSize[20];
		recvfrom(server, creationTime, 30, 0, (sockaddr*)&remote_addr, &nAddrlen);
		recvfrom(server, fileSize, 20, 0, (sockaddr*)&remote_addr, &nAddrlen);
		cout << "Creation Time：" << creationTime << endl;
		cout << "Size：" << fileSize << endl;
		cout << "Successfully received!" << endl;
		fclose(fp);
	}
	closesocket(server);
	WSACleanup();
	return 0;
}
