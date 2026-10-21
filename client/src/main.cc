#include "networking.hh"
#include "data.hh"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <ostream>

uint64_t authToken = 0x0000000000000000;
uint32_t playerId = 100;

std::vector<Data> saveDataFile;
std::vector<uint8_t> dataBuffer;

Data dataBug;

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    saveDataFile.push_back( {300, 2050, 100});

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(55554);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("connect");
        close(clientSocket);
        return 1;
    }

    Packet FirstAuth(Packet::AUTH, authToken, playerId);
    std::vector<uint8_t> authData (1024);
    FirstAuth.SerializePacket(authData);
    send(clientSocket, authData.data(), authData.size(), 0);
    explicit_bzero(authData.data(), authData.size());

    ssize_t bytesReceived = recv(clientSocket, authData.data(), authData.size(), 0);
    authData.resize(sizeof(NetworkPacket));
    FirstAuth.DeserializePacket(authData);

    NetworkPacket authPacket = FirstAuth.GetPacket();
    authToken = authPacket.Token;
    playerId = authPacket.playerId;

    while (true) {
        std::vector<uint8_t> buffer;
        std::vector<uint8_t> receiveBuffer(1024);

        std::string answer;
        std::cout << "Send packet with correct data? y/n" << std::endl;
        std::getline(std::cin, answer);

        if (answer == "y") {
            Packet RealPacket(Packet::AUTH, authToken, playerId);
            RealPacket.SerializePacket(buffer);
        } else if (answer == "n") {
            Packet FakePacket(Packet::AUTH, 0x0123456789abcdef, playerId);
            FakePacket.SerializePacket(buffer);
        } else if (answer == "save") {
            data::saveData("../../../Server/ServerTests/file?.bon", saveDataFile);
            continue;
        } else if (answer == "read") {
            data::readData("../../../Server/ServerTests/file?.bon", dataBuffer);

            std::memcpy(&dataBug, dataBuffer.data(), sizeof(Data));

            std::cout << "read this: " << dataBug.id << " " << dataBug.something << " " << dataBug.value << std::endl;
            continue;
        } else {
            send(clientSocket, "Bye!", 5, 0);
            break;
        }

        ssize_t bytesSent = send(clientSocket, buffer.data(), buffer.size(), 0);
        std::cout << "Sent: " << bytesSent << " bytes" << std::endl;

        ssize_t receivedBytes = recv(clientSocket, receiveBuffer.data(), receiveBuffer.size(), 0);
        receiveBuffer.resize(receivedBytes);
        Packet IncomingPacket;
        IncomingPacket.DeserializePacket(receiveBuffer);
        NetworkPacket packet = IncomingPacket.GetPacket();
    }

    close(clientSocket);

    return 0;
}