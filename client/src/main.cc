#include "networking.hh"
#include <iostream>

Packet AuthPacket(Packet::AUTH, 987654321ULL, 101);
Packet DataPacket(Packet::DATA, 987654321ULL, 102);
Packet ChatPacket(Packet::CHAT, 987654321ULL, 103, 0, "Hello Server");

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(55554);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("connect");
        close(clientSocket);
        return 1;
    }

    while (true) {
        std::vector<uint8_t> buffer;

        std::string sendPacketType;
        std::cout << "What type of packet to send?" << std::endl;
        std::cin >> sendPacketType;
        if (sendPacketType == "AUTH") {
            AuthPacket.SerializePacket(AuthPacket.GetPacket(), buffer);
        } else if (sendPacketType == "DATA") {
            DataPacket.SerializePacket(DataPacket.GetPacket(), buffer);
        } else if (sendPacketType == "CHAT") {
            ChatPacket.SerializePacket(ChatPacket.GetPacket(), buffer);
        }

        ssize_t bytesSent = send(clientSocket, buffer.data(), buffer.size(), 0);
        std::cout << "Sent: " << bytesSent << " bytes" << std::endl;
    }

    close(clientSocket);

    return 0;
}