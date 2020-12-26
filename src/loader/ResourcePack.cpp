#include "loader/ResourcePack.hpp"

namespace RIS::Loader
{
    std::vector<std::byte> ResourcePack::Read(const std::string &res) const
    {
        for(const PackType& p : packs)
        {
            const Pack &pack = std::visit([](auto &&v) -> const Pack& { return v; }, p);
            if(pack.Contains(res))
                return pack.Read(res);
        }
        return {};
    }
}
