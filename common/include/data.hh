#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

#pragma pack(push, 1)
struct Data {
    uint32_t id;
    uint64_t value;
    uint16_t something;
};
#pragma pack(pop)

namespace data {
    inline void saveData(std::string file, std::vector<Data> data) {
        std::ofstream out(file, std::ios::binary);

        out.write(reinterpret_cast<char*>(data.data()), data.size() * sizeof(Data));

        std::cout << "Wrote: " << data.size() * sizeof(Data) << "Bytes" << std::endl;
    }

    inline void readData(std::string file, std::vector<uint8_t> &buffer) {
        std::ifstream in(file, std::ios::binary | std::ios::ate);

        const std::streamsize size = in.tellg();
        in.seekg(0, std::ios::beg);

        buffer.resize(static_cast<size_t>(size));
        in.read(reinterpret_cast<char*>(buffer.data()), size);
    }
}