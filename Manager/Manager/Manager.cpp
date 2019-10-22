#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>

#include <tchar.h>
#include <cstdlib>
#include <future> 
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

const int N = 2;
const int _SIZE = 20;

bool type = true;//false - esc; true - periodic prompt

SOCKET sockets[N];
std::vector<PROCESS_INFORMATION> processInfo;
int values[N] = {};
bool wasCalculated[N] = {};
std::future<void> futures[N];

bool exitByEsc = false;
bool isInMenu = false;
bool isCalculated;

std::future<void> f;

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

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	if (sListen == INVALID_SOCKET)
	{
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		exit(1);
	}
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	for (SOCKET &socket : sockets) {
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (!CreateProcess(NULL, _tcsdup(TEXT("\"..\\..\\Functions\\Debug\\Functions\" ")),
			NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
			std::cout << "error create process";
			exit(1);
		}
		processInfo.push_back(pi);
		socket = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
		
	}
	closesocket(sListen);
}

void wait(int index) {
	char* ch = new char[_SIZE];
	ZeroMemory(ch, _SIZE);
	if (recv(sockets[index], ch, sizeof(ch), NULL) != SOCKET_ERROR) {
		wasCalculated[index] = true;
	}
	values[index]=atoi(ch);
	delete[] ch;
}

void closeSocket() {
	// Gracefully close down everything
	for(SOCKET socket : sockets)
		closesocket(socket);
	WSACleanup();
	processInfo.clear();
}

void action(int c);

void writeMenu() {
	isInMenu = true;
	std::cout << "Options:\n"
		<< " a) continue\n b) continue without prompt\n c) cancel\n";
	int c;
	do {
		c = getche();
		std::cout << "\n";
		action(c);
	} while (c < 'a' || c >'c');
}

void waitForNextCall() {
	std::this_thread::sleep_for(std::chrono::seconds(2));
	if (!isCalculated)
		writeMenu();
}

void closeAllFunctions(){
	//close all functions no calculated
	for (int i = 0; i < N; i++)
		if (!wasCalculated[i]) {
			TerminateProcess(processInfo[i].hProcess, 0);
		}
}


int getAnswer(bool& isCalculated, bool isInMenu, bool isExitByC) {
	if (isInMenu) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
		return 1;
	}
	int answer = 1;
	bool isCalculatedL = true;
	for (int i = 0; i < N; i++) {
		if ((futures[i].wait_for(std::chrono::milliseconds(1)) == std::future_status::ready) && !exitByEsc && !isInMenu) {
			if (values[i] == 0) {
				isCalculatedL = true;
				answer = 0;
				
				if (!isExitByC)
					std::cout << "answer: 0 " << (i == 0 ? "f" : "g") << " is zero;\n";
				break;
			}
		}
		else {
			isCalculatedL = false;
		}
	}

	if (isCalculatedL && !exitByEsc && !isInMenu) {
		if (answer == 0) {}
		else {
			answer = abs(values[0]);

			for (int i = 1; i < N; i++)
				if (answer > abs(values[i]))
					answer = abs(values[i]);
			
			if (!isExitByC)
				std::cout << "answer: " << answer << ";\n";
		}
	}
	if (isCalculatedL)
		isCalculated = isCalculatedL;
	return answer;
}

void writeWhyIsNotAnswer() {
	std::cout << "Is not answer becouse "
		<< (wasCalculated[0] ? "" : "f ")
		<< (wasCalculated[1] ? "" : "g ")
		<< "is not calculated \n";
}


void action(int c) {

	if (c == 'a') {
		bool isCalculatedL = false;
		int answer = getAnswer(isCalculatedL, false, true);
		if (!isCalculatedL)
			f = std::async(waitForNextCall);
	}
	if (c == 'a'|| c == 'b') {
		isInMenu = false;
	}
	else if (c == 'c') {
		bool isCalculatedL = false;
		int answer = getAnswer(isCalculatedL, false, true);
		if (isCalculatedL)
			std::cout << "STOOOOP. Answer is calculated! Answer is " << answer << std::endl;
		else {
			writeWhyIsNotAnswer();
		}
		isCalculated = true;
	}
}


void isEsc() {
	while (!isCalculated) {
		int z = getch();
		if (isCalculated) return;
		if (z == 27) {
			isCalculated = true;
			exitByEsc = true;
			writeWhyIsNotAnswer();
		}
	}
	
}


void checkAnswer() {
	isCalculated = false;
	while (!isCalculated) {
		getAnswer(isCalculated, isInMenu, false);
	}
	closeAllFunctions();
	closeSocket();
}


void restart() {
	processInfo.clear();	
	exitByEsc = false;
	isInMenu = false;
	isCalculated = false;
	for (int i=0;i<N;i++) 
		wasCalculated[i] = false;
	
}



int main(int argc, char* argv[]) 
{
	for (int i = 0; i < 10; i++) {
		
		int x;
		std::cout << "x = ";
		std::cin >> x;
		std::cin.clear();
		std::cin.ignore(10000, '\n');
		char* c = new char[_SIZE];
		ZeroMemory(c, _SIZE);
		
		createSocket();
		for (SOCKET socket : sockets)
			if (socket == 0) {
				std::cout << "Error #1\n" << socket;
				exit(1);
			}
		
		for (int i = 0; i < N; i++) {
			itoa(i, c, 10);
			send(sockets[i], c, sizeof(c), NULL);
			ZeroMemory(c, _SIZE);
		}
		itoa(x, c, 10);
		for (int i = 0; i < N; i++) {
			send(sockets[i], c, sizeof(c), NULL);
		}
		delete[] c;

		for (int i = 0; i < N; i++) {
			futures[i] = std::async(wait, i);
		}

		if (type)
			f = std::async(&waitForNextCall);
		else
			f = std::async(&isEsc);

		checkAnswer();

		if (!type && exitByEsc) {
			system("pause");
		}
		else {
			std::cout << "press any key to continue... \n";
		}

		f.wait();
		restart();

	}

	
}