#pragma once

#include <vector>
#include <string>

namespace RIS::Loader
{
    class Pack
    {
    public:
        virtual ~Pack() = default;

        virtual std::vector<std::byte> Read(const std::string &res) const = 0;
        virtual bool Contains(const std::string &res) const = 0;

    };
}
