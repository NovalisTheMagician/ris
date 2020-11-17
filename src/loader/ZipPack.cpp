#include "loader/ZipPack.hpp"

#include <filesystem>

namespace RIS
{
    namespace Loader
    {
        ZipPack::ZipPack(const std::string &archivePath)
            : archive(archivePath)
        {}

        std::vector<std::byte> ZipPack::Read(const std::string &res) const
        {
            std::filesystem::path filePath = res;
            int64_t fileIndex = archive.find(filePath.generic_string(), ZIP_FL_NOCASE);
            libzip::stat fileStat = archive.stat(fileIndex);
            std::size_t size = static_cast<std::size_t>(fileStat.size);
            
            std::vector<std::byte> bytes(size);

            libzip::file file = const_cast<libzip::archive&>(archive).open(fileIndex);
            if(file.read(bytes.data(), fileStat.size) == -1)
                return {};
            return bytes;
        }

        bool ZipPack::Contains(const std::string &res) const
        {
            return archive.exists(res, ZIP_FL_NOCASE);
        }
    }
}
