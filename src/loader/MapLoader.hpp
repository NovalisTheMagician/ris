#pragma once

#include "loader/LoadFunc.hpp"

#include <memory>

namespace RIS::Graphics { class MapMesh; }

namespace RIS::Loader
{
    template<>
    std::shared_ptr<Graphics::MapMesh> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack);
}
