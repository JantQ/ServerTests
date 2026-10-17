#include <cstring>
#include <iostream>
#include <print>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "networking.hh"
#include <string>

int main() {
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(55555);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	
	connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	
	while (true) {
		std::string answer;
		std::println("Send Packet? y/n");
		std::getline(std::cin, answer);

		if (answer == "y") {
			sendPacket(clientSocket);
			receivePacket(clientSocket);
		}
		else if (answer == "exit") {
			break;
		}
	}
	close(clientSocket);

	return 0;
}

void sendPacket(int clientSocket) {
	const char* message = "Hello World of the networking!";
	send(clientSocket, message, strlen(message), 0);
}

void receivePacket(int clientSocket) {
	char buffer[1024] = {0};
	recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	std::println("Message from the server: {}", buffer);
}
