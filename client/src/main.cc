#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "common/networking.hh"
#include <string>

int main() {
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	GamePacket packet;
	packet.packetType = 1;
	packet.playerId = 1234;

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(55554);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	
	connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	
	while (true) {
		std::string answer;
		std::cout << "Send Packet? y/n\n";
		std::getline(std::cin, answer);

		if (answer == "y") {
			sendPacket(clientSocket, packet);
			receivePacket(clientSocket);
		}
		else if (answer == "exit") {
			packet.packetType = PACKET_DISCONNECT;
			packet.playerId = 0;
			sendPacket(clientSocket, packet);
			receivePacket(clientSocket);
			break;
		}
	}
	close(clientSocket);

	return 0;
}

void sendPacket(int clientSocket, GamePacket packet) {
	send(clientSocket, &packet, sizeof(packet), 0);
}

void receivePacket(int clientSocket) {
	char buffer[1024] = {0};
	recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	std::cout << "Message from the server: " << buffer << '\n';
}
