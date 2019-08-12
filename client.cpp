/*
客户端程序
客户端给服务端发送文件，包含管理元数据（文件名、大小和日期）
*/
#include<iostream>
#include<WinSock2.h>
#include<winsock.h>
#include<Windows.h>
#include<string>
#include<cstring>
#include <fstream>
#include <io.h>
#pragma comment(lib,"ws2_32.lib")
#define BUF_SIZE 1024
#define SERVER_ID "127.0.0.1"
#define PATH_LENGTH 20
using namespace std;
char sendBuff[BUF_SIZE];
char recvBuff[BUF_SIZE];
char fileName[PATH_LENGTH];

BOOL getFileTime(HANDLE hFile, LPSTR lpszCreationTime)//获取文件创建日期
{
	FILETIME ftCreate, ftAccess, ftWrite;
	SYSTEMTIME stUTC1, stLocal1;

	// -------->获取 FileTime
	if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
		cout << "error!" << endl;
		return FALSE;
	}
	//---------> 转换: FileTime --> LocalTime
	FileTimeToSystemTime(&ftCreate, &stUTC1);

	SystemTimeToTzSpecificLocalTime(NULL, &stUTC1, &stLocal1);

	// ---------> Show the  date and time.
	sprintf(lpszCreationTime, "%02d/%02d/%02d  %02d:%02d",
		stLocal1.wYear, stLocal1.wMonth, stLocal1.wDay,
		stLocal1.wHour, stLocal1.wMinute);
	return TRUE;
}

int main() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout << "Initialization failed." << endl;
		return -1;
	}
	SOCKET client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (client == -1) {
		cout << "Socket failed." << endl;
		return -1;
	}
	sockaddr_in sadr;
	sadr.sin_family = AF_INET;
	sadr.sin_port = htons(5000);
	sadr.sin_addr.S_un.S_addr = inet_addr(SERVER_ID);
	int nAddrlen = sizeof(sadr);
	while (true) {
		cout << "---------------------SENDING...---------------------" << endl;
		cout << "Please input the filename: " << endl;
		cin >> fileName;
		FILE *fp;
		if (!(fp = fopen(fileName, "rb"))) {
			cout << "Fail to open file." << endl;
			continue;
		}
		//先传送文件名
		sendto(client, fileName, strlen(fileName), 0, (sockaddr*)&sadr, sizeof(sadr));
		int length;
		int ret;
		while ((length = fread(sendBuff, 1, BUF_SIZE, fp)) > 0) {
			ret = sendto(client, sendBuff, length, 0, (sockaddr*)&sadr, sizeof(sadr));
			if (!ret) {
				cout << "An error occurred while sending." << endl;
				return -1;
			}
			ret = recvfrom(client, recvBuff, BUF_SIZE, 0, (sockaddr*)&sadr, &nAddrlen);
			if (!ret) {
				cout << "Fail to receive." << endl;
				return -1;
			}
			else {
				if (strcmp(recvBuff, "success")) {
					cout << "Fail to receive." << endl;
					return -1;
				}
			}
		}
		//传送文件发送结束信息
		char end_flag[10] = "end";
		ret = sendto(client, end_flag, length, 0, (sockaddr*)&sadr, sizeof(sadr));
		//获取文件创建时间和大小
		HANDLE hFile;
		TCHAR szCreationTime[30];
		hFile = CreateFile(fileName, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		getFileTime(hFile, szCreationTime);
		if (hFile == INVALID_HANDLE_VALUE) {
			cout << "error!" << endl;
			return -1;
		}
		int size = GetFileSize(hFile, NULL);
		//传送时间和大小信息
		ret = sendto(client, szCreationTime, 30, 0, (sockaddr*)&sadr, sizeof(sadr));
		size = size / 1024 + 1;//B转KB
		string tempSize = to_string(size);
		tempSize += "KB";
		char fileSize[20];
		strcpy(fileSize, tempSize.c_str());
		ret = sendto(client, fileSize, 20, 0, (sockaddr*)&sadr, sizeof(sadr));
		cout << "successfully sent!" << endl;
		fclose(fp);
		CloseHandle(hFile);
	}
	closesocket(client);
	WSACleanup();
	return 0;
}
