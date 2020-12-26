#include "loader/FilesystemPack.hpp"

#include <fstream>

namespace RIS::Loader
{
    FilesystemPack::FilesystemPack(const std::string &folder)
        : folder(folder)
    {}

    std::vector<std::byte> FilesystemPack::Read(const std::string &res) const
    {
        auto path = folder / res;
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if(file)
        {
            std::size_t size = file.tellg();
            file.seekg(0);
            std::vector<std::byte> bytes(size);
            file.read(reinterpret_cast<char*>(bytes.data()), size);

            return bytes;
        }
        return {};
    }

    bool FilesystemPack::Contains(const std::string &res) const
    {
        auto path = folder / res;
        return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
    }
}
