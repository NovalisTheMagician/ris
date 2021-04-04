#pragma once

#include <string>
#include <vector>
#include <any>
#include <memory>

#include "loader/ResourcePack.hpp"
#include "loader/AssetCache.hpp"

namespace RIS::Loader
{
    namespace
    {
        static AssetCache cache;
    }
    inline AssetCache& GetCache() { return cache; }

    template<typename T>
    std::shared_ptr<T> Load(const std::string &res, const ResourcePack &resourcePack, std::any param = {})
    {  
        auto &cache = GetCache();
        if(cache.IsCached<T>(res))
        {
            cache.AddRef<T>(res);
            return cache.Get<T>(res);
        }

        auto bytes = resourcePack.Read(res);
        if(!bytes.empty())
        {
            auto asset = Load<T>(bytes, res, param, resourcePack);
            cache.Cache<T>(res, asset);
            return asset;
        }
        return nullptr;
    }
}
