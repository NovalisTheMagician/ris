#pragma once 

#include "loader/Pack.hpp"

#include <string>
#include <map>

namespace RIS
{
    namespace Loader
    {
        class EmbeddedPack : public Pack
        {
        public:

            std::vector<std::byte> Read(const std::string &res) const override;
            bool Contains(const std::string &res) const override;

        private:
            static const std::map<std::string, std::string> resources;

        };
    }
}
