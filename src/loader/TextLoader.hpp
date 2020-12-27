#pragma once

#include "loader/LoadFunc.hpp"

#include <string>

namespace RIS::Loader
{
    template<>
    std::shared_ptr<std::string> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack);
}
