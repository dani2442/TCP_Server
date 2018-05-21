/*
CMD IMPORTANT COMMANDS
tracert  : provides us the principal hostname of a website
arp -a   : provides us the status (dynamic or static) ip in our local ip
ipconfig : provides the actual ip of our computer
*/

#include <string>
#include <iostream>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

int main() {

	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0) {
		cerr << "can`t Initialize winsock " << endl;
		return 1;
	}

	// Create a socket 
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		cerr << "can`t create a socket <<endl";
		return 1;
	}

	// Bind ip adderess and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; //could also use inet_pton...

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//Tell wonsock the socket is for listening
	listen(listening, SOMAXCONN);

	//wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host << " connected on port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}

	// Clse listening socket 
	closesocket(listening);

	// Whole loop: accept and echo message back to client
	char buf[4096];
	while (true) {
		ZeroMemory(buf, 4096);

		//Wait for client to sned data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Error in recv()." << endl;
			break;
		}
		if (bytesReceived == 0) {
			cout << "Client disconnected " << endl;
			break;
		}

		cout << string(buf, 0, bytesReceived)<<endl;
		//Echo message back to client
		send(clientSocket, buf, bytesReceived + 1, 0);
	}

	// Close socket 
	closesocket(clientSocket);
	// Shutdown winock
	WSACleanup();
}