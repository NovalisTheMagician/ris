#pragma once

#include "RisExcept.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <any>
#include <optional>
#include <memory>

#include "misc/Logger.hpp"

#include <zip.hpp>

#include "loader/LoadFunc.hpp"
#include "loader/ResourcePack.hpp"

namespace RIS::Loader
{
    template<typename T>
    std::shared_ptr<T> Load(const std::string &res, const ResourcePack &resourcePack, std::any param = {})
    {
        auto bytes = resourcePack.Read(res);
        if(!bytes.empty())
        {
            return Load<T>(bytes, res, param, resourcePack);
        }
        return nullptr;
    }
}
