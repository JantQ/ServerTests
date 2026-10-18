#pragma once

#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>

#pragma pack(push, 1)
struct NetworkPacket {
    uint8_t packetType : 3;
    uint8_t reserved : 5;
    uint64_t Token;
    uint32_t playerId;
};
#pragma pack(pop)

class Packet {
public:
    enum PacketType : uint8_t {
        AUTH = 0x00,
        DATA = 0x01,
        CHAT = 0x02
    };

    Packet(PacketType type = AUTH, uint64_t token = 0, uint32_t playerId = 0, uint32_t value = 0, std::string txt = {}) {
        packet.packetType = type;
        packet.reserved = 0;
        packet.Token = token;
        packet.playerId = playerId;

        if (type == CHAT) {
            text = txt;
        }
    }

    Packet() = default;

    void SerializePacket(const NetworkPacket& packet, std::vector<uint8_t> &buffer) {
        buffer.resize(sizeof(NetworkPacket));
        std::memcpy(buffer.data(), &packet, sizeof(NetworkPacket));

        if (packet.packetType == CHAT) {
            uint16_t textLength = static_cast<uint16_t>(text.size());

            buffer.insert(buffer.end(), {
                static_cast<uint8_t>(textLength & 0xFF),
                static_cast<uint8_t>((textLength >> 8) & 0xFF)
            });
            buffer.insert(buffer.end(), text.begin(), text.end());
        }
    }

    void DeserializePacket(const std::vector<uint8_t> &buffer) {
        if (buffer.size() < sizeof(NetworkPacket)) {
            std::cout << "Buffer size is too small to contain a valid NetworkPacket." << std::endl;
            return;
        }

        std::memcpy(&packet, buffer.data(), sizeof(NetworkPacket));

        if (packet.packetType == CHAT) {
            size_t offset = sizeof(NetworkPacket);

            if (buffer.size() < offset + 2) {
                std::cout << "Buffer too small to text" << std::endl;
                return;
            }

            uint16_t textLength = 0;
            textLength |= static_cast<uint16_t>(buffer[offset]);
            textLength |= static_cast<uint16_t>(buffer[offset + 1]) << 8;
            offset += 2;

            text.assign(reinterpret_cast<const char*>(buffer.data() + offset), textLength);
        }
    }

    const NetworkPacket& GetPacket() const { return packet; }
    const std::string& GetText() const { return text; }
    const uint32_t& GetValue() const { return value; }

private:
    NetworkPacket packet;
    std::string text;
    uint32_t value;
};