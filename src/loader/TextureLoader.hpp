#pragma once

#include "loader/LoadFunc.hpp"

#include <memory>

namespace RIS::Graphics { class Texture; }

namespace RIS::Loader
{
    template<>
    std::shared_ptr<Graphics::Texture> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack);
}
