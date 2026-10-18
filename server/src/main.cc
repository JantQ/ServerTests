#include "networking.hh"

std::vector<int> clientSockets;

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

                incomingPacket.DeserializePacket(receiveBuffer);

                NetworkPacket packet = incomingPacket.GetPacket();

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
