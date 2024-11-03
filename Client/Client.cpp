#include <string>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>

#define PORT 8080
#pragma comment(lib, "ws2_32.lib")
#define ADDRESS "192.168.50.155"

using namespace std;

void receiveMessage(SOCKET sock) {
	char buffer[1024];
	while (true) {
		int recv_size = recv(sock, buffer, sizeof(buffer), 0);
		if (recv_size <= 0) {
			cerr << "Disconnected from server" << endl;
			closesocket(sock);
			break;
		}
		buffer[recv_size] = '\0';
		cout << "\r" << buffer << endl;
		cout << ">> ";
		cout.flush();
	}
}
int main() {
	cout << "CLIENT" << endl;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cerr << "WSAstartup failed : " << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}
	else {
		cout << "WSAStartup was successful" << endl;
	}

	struct sockaddr_in server_addr;
	char buffer[1024] = { 0 };

	SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		cerr << "Failed to create server_fdet : " << WSAGetLastError() << endl;
		return -1;
	}
	else {
		cout << "Creating the server_fdet was successful." << endl;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, ADDRESS, &server_addr.sin_addr) <= 0) {
		cerr << "Invalid address/ Address not supported : " << ADDRESS << endl;
		return -1;
	}

	if (connect(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
		cerr << "Failed connect to server : " << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}
	else {
		cout << "Connecting to the server was successful." << endl;
	}

	string nickname;
	cout << "Your name :";
	getline(cin, nickname);

	if (send(server_fd, nickname.c_str(), nickname.size(), 0) == SOCKET_ERROR) {
		cerr << "Failed to send nickname : " << WSAGetLastError() << endl;
		closesocket(server_fd);
		WSACleanup();
		return -1;
	}

	thread(receiveMessage, server_fd).detach();
	string message;
	while (true) {
		cout << ">> ";
		getline(cin, message);

		if (message == "exit") {
			cout << "Disconnecting from server..." << endl;
			break;
		}

		string fullMessage = message;
		if (send(server_fd, fullMessage.c_str(), fullMessage.size(), 0) == SOCKET_ERROR) {
			cerr << "Failed to send message : " << WSAGetLastError() << endl;
			break;
		}
	}

	closesocket(server_fd);
	WSACleanup();

	return 0;
}