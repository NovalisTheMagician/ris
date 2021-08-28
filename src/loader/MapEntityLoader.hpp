#pragma once

#include "loader/LoadFunc.hpp"

#include <memory>
#include <vector>

namespace RIS::Game
{
    class MapEntity;
}

namespace RIS::Loader
{
    template<>
    std::shared_ptr<std::vector<Game::MapEntity>> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack);
}
