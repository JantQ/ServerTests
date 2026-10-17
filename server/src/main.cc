#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include "server/networking.hh"
#include "common/networking.hh"

int main() {
	GamePacket packet;
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(55554);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	
	listen(serverSocket, 5);
	int clientSocket = accept(serverSocket, nullptr, nullptr);

    while (true) {
        ssize_t bytesRead = recv(clientSocket, &packet, sizeof(packet), 0);

        if (bytesRead <= 0) {
            std::cout << "Error receiving packet from client.\n";
            close(clientSocket);
            while ((clientSocket = accept(serverSocket, nullptr, nullptr)) < 0) {

            }
        }

        if (packet.packetType == PACKET_DISCONNECT) {
            std::cout << "Client " << packet.playerId << " requested disconnect.\n";
            send(clientSocket, "Disconnecting from server.\n", 27, 0);
            close(clientSocket);
            
            while ((clientSocket = accept(serverSocket, nullptr, nullptr)) < 0) {

            }
        }

        std::cout << "Received message from client " << packet.playerId << " with type " << (int)packet.packetType << ".\n";
        std::string response = "Hello from the server! I received your message.\n";
        send(clientSocket, response.c_str(), response.length(), 0);
    }
	close(serverSocket);
	return 0;
}