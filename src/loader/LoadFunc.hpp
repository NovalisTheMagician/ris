#pragma once

#include <vector>
#include <memory>
#include <any>
#include <string>

#include "loader/ResourcePack.hpp"

namespace RIS
{
    namespace Loader
    {
        template<typename T>
        std::shared_ptr<T> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack);
    }
}
