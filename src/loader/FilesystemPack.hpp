#pragma once

#include "loader/Pack.hpp"

#include <filesystem>

namespace RIS
{
    namespace Loader
    {
        class FilesystemPack : public Pack
        {
        public:
            FilesystemPack(const std::string &folder);
            virtual ~FilesystemPack() = default;

            FilesystemPack(const FilesystemPack&) = delete;
            FilesystemPack& operator=(const FilesystemPack&) = delete;
            FilesystemPack(FilesystemPack&&) = default;
            FilesystemPack& operator=(FilesystemPack&&) = default;

            std::vector<std::byte> Read(const std::string &res) const override;
            bool Contains(const std::string &res) const override;

        private:
            std::filesystem::path folder;

        };
    }
}
