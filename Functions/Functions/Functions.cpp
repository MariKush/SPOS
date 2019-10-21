#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <functional>
#include "demofuncs"
#pragma warning(disable: 4996)

const int _SIZE = 20;

SOCKET Connection;

void createSocket() {
	//downloading the library
	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	//fill in socket address information
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		std::cout << "Error: failed connect to server.\n";
		exit(0);
	}
}

void closeSocket() {
	// Gracefully close down everything
	closesocket(Connection);
	WSACleanup();
}

int main() {
	HWND hWnd = GetForegroundWindow();
	//ShowWindow(hWnd, SW_HIDE);
	createSocket();

	char *c = new char[_SIZE];
	ZeroMemory(c, _SIZE);
	recv(Connection, c, sizeof(c), NULL);

	int ind = atoi(c);

	ZeroMemory(c, _SIZE);
	recv(Connection, c, sizeof(c), NULL);

	int x = atoi(c);

	int answer;

	if (ind == 0) {
		answer = spos::lab1::demo::f_func<spos::lab1::demo::INT>(x);
	}
	else {
		answer = spos::lab1::demo::g_func<spos::lab1::demo::INT>(x);
	}

	ZeroMemory(c, _SIZE	);

	itoa(answer, c, 10);
	send(Connection, c, sizeof(c), NULL);
	closeSocket();

	return 0;
}