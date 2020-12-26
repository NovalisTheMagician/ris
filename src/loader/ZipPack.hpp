#pragma once

#include "loader/Pack.hpp"

#include <zip.hpp>

namespace RIS::Loader
{
    class ZipPack : public Pack
    {
    public:
        ZipPack(const std::string &archivePath);
        virtual ~ZipPack() = default;

        ZipPack(const ZipPack &) = delete;
        ZipPack& operator=(const ZipPack &) = delete;
        ZipPack(ZipPack &&) = default;
        ZipPack& operator=(ZipPack &&) = default;

        std::vector<std::byte> Read(const std::string &res) const override;
        bool Contains(const std::string &res) const override;

    private:
        libzip::archive archive;

    };
}
