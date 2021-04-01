#include "loader/EmbeddedPack.hpp"

#include <base64.h>

namespace RIS::Loader
{
    const std::map<std::string, std::string> EmbeddedPack::resources = 
    {
        {"test", "SGVsbG8gV29ybGQgRW1iZWRkZWQh"}
    };

    std::vector<std::byte> EmbeddedPack::Read(const std::string &res) const
    {
        if(Contains(res))
        {
            try
            {
                std::string resource = base64_decode(resources.at(res));
                std::vector<std::byte> bytes(resource.size());
                for(std::size_t i = 0; i < resource.size(); ++i)
                    bytes[i] = static_cast<std::byte>(resource.at(i));
                return bytes;
            }
            catch(const std::exception&)
            {}
        }
        return {};
    }

    bool EmbeddedPack::Contains(const std::string &res) const
    {
        return resources.count(res) > 0;
    }
}
