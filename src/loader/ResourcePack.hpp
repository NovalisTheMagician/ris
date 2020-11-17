#pragma once

#include <vector>
#include <string>
#include <variant>

#include "loader/ZipPack.hpp"
#include "loader/FilesystemPack.hpp"

namespace RIS
{
    namespace Loader
    {
        using PackType = std::variant<ZipPack, FilesystemPack>;

        class ResourcePack
        {
        public:
            template<typename P>
            void PushFront(P &&pack);
            template<typename P>
            void PushBack(P &&pack);

            std::vector<std::byte> Read(const std::string &res) const;

        private:
            std::vector<PackType> packs;

        };

        template<typename P>
        void ResourcePack::PushFront(P &&pack)
        {
            packs.insert(packs.begin(), std::forward<P>(pack));
        }

        template<typename P>
        void ResourcePack::PushBack(P &&pack)
        {
            packs.push_back(std::forward<P>(pack));
        }
    }
}
