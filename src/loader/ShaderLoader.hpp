#pragma once

#include "loader/LoadFunc.hpp"

#include <memory>

namespace RIS::Graphics { class Shader; }

namespace RIS::Loader
{
    template<>
    std::shared_ptr<Graphics::Shader> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack);
}
