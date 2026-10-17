#pragma once

#include <string>

#define PACKET_DISCONNECT 0

#pragma pack(push, 1)
struct GamePacket {
    uint8_t packetType : 3;

    uint32_t playerId;
};
#pragma pack(pop)

void sendPacket(int clientSocket, GamePacket packet);
void receivePacket(int clientSocket);