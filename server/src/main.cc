#include "networking.hh"
#include <iostream>
#include <sys/types.h>

std::vector<int> clientSockets;

struct Vector2 {
    uint32_t playerId;
    uint64_t authToken;
};
std::vector<Vector2> PlayerAuthList;

int main() {
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(55554);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    
    if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl (set O_NONBLOCK) on server socket");
        close(serverSocket);
        return 1;
    }

	listen(serverSocket, 5);
    PlayerAuthList.push_back({100, 0});

    while (true) {
        fd_set readfds;
        FD_ZERO(&readfds);

        FD_SET(serverSocket, &readfds);
        int maxfd = serverSocket;

        for (int socket : clientSockets) {
            FD_SET(socket, &readfds);
            if (socket > maxfd) {
                maxfd = socket;
            }
        }

        int ready = select(maxfd + 1, &readfds, nullptr, nullptr, nullptr);
        if (ready < 0) continue;

        if (FD_ISSET(serverSocket, &readfds)) {
            int newClientSocket = accept(serverSocket, nullptr, nullptr);


            std::cout << "New client connected: " << newClientSocket << std::endl;
            if (fcntl(newClientSocket, F_SETFL, O_NONBLOCK) == -1) {
                perror("fcntl (set O_NONBLOCK) on client socket");
                close(newClientSocket);
                continue;
            }
            clientSockets.push_back(newClientSocket);
        }

        for (auto it = clientSockets.begin(); it != clientSockets.end();) {
            int socket = *it;
            if (FD_ISSET(socket, &readfds)) {
                std::vector<uint8_t> receiveBuffer(1024);
                ssize_t bytesReceived = recv(socket, receiveBuffer.data(), receiveBuffer.size(), 0);
                
                if (bytesReceived <= 0) {
                    std::cout << "Client Disconnected" << std::endl;
                    close(socket);
                    it = clientSockets.erase(it);
                    continue;
                }

                Packet incomingPacket;
                receiveBuffer.resize(bytesReceived);

                bool real_packet = incomingPacket.DeserializePacket(receiveBuffer);

                if (!real_packet) {
                    close(socket);
                    it = clientSockets.erase(it);
                    continue;
                }

                NetworkPacket packet = incomingPacket.GetPacket();

                bool playerFound = false;
                bool isAuthenticated = false;
                for (auto& player : PlayerAuthList) {
                    if (player.playerId == packet.playerId) {
                        playerFound = true;

                        if (player.authToken != 0 && player.authToken == packet.Token) {

                            isAuthenticated = true;
                            std::cout << "Player Authenticated" << std::endl;
                            Packet AuthPacket(Packet::AUTH, player.authToken, player.playerId);
                            std::vector<uint8_t> buffer;
                            AuthPacket.SerializePacket(buffer); 
                            send(socket, buffer.data(), buffer.size(), 0);

                        } else if (player.authToken == 0) {

                            player.authToken = GenerateNewToken();
                            Packet AuthPacket(Packet::AUTH, player.authToken, packet.playerId);
                            std::vector<uint8_t> buffer;
                            AuthPacket.SerializePacket(buffer); 
                            send(socket, buffer.data(), buffer.size(), 0);

                        } else if (player.authToken != packet.Token) {

                            isAuthenticated = false;
                            std::cout << "Player is not authenticated" << std::endl;
                            Packet AuthPacket(Packet::AUTH, packet.Token, packet.playerId);
                            std::vector<uint8_t> buffer;
                            AuthPacket.SerializePacket(buffer); 
                            send(socket, buffer.data(), buffer.size(), 0);
                        }
                        break;
                    }
                }

                if (!playerFound) {
                    std::cout << "Player not found from authlist" << std::endl;
                }

                std::cout << "Packet Type: " << +packet.packetType << std::endl;
                if (packet.packetType == Packet::CHAT) {
                    std::cout << "Received: " << incomingPacket.GetText() << std::endl;
                }

                ++it;
            } else {
                ++it; 
            }
        }
    }
	close(serverSocket);
	return 0;
}
