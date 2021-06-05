#pragma once

#include "loader/LoadFunc.hpp"

#include <memory>

namespace RIS::Physics
{
    class WorldSolids;
}

namespace RIS::Loader
{
    template<>
    std::shared_ptr<Physics::WorldSolids> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack);
}
