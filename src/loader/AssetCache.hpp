#pragma once

#include "loader/Loaders.hpp"

#include <variant>
#include <unordered_map>
#include <experimental/unordered_map>
#include <memory>
#include <string>
#include <algorithm>
#include <typeinfo>

#include <fmt/format.h>

namespace RIS
{
    namespace Physics
    {
        class WorldSolids;
    }

    namespace Graphics
    {
        class Model;
        class Mesh;
        class Font;
        class Image;
        class Shader;
        class Texture;
        class MapMesh;
        namespace Animation
        {
            class Animation;
            class Skeleton;
        }
    }
    
}

namespace RIS::Loader
{
    template<typename... AssetTypes>
    class TAssetCache
    {
    private:
        using TVariant = std::variant<std::shared_ptr<AssetTypes>...>;
        struct RefCount
        {
            TVariant asset;
            int count;
        };

    public:
        template<typename AssetType>
        bool IsCached(const std::string &name) const
        {
            return assets.count(GetTypeName<AssetType>(name)) > 0;
        }

        template<typename AssetType>
        void AddRef(const std::string &name)
        {
            auto tname = GetTypeName<AssetType>(name);
            if(IsCached<AssetType>(name))
                assets.at(tname).count++;
        }

        template<typename AssetType>
        void Cache(const std::string &name, std::shared_ptr<AssetType> asset)
        {
            RefCount rc{ asset, 1 };
            assets.insert_or_assign(GetTypeName<AssetType>(name), rc);
        }

        template<typename AssetType>
        std::shared_ptr<AssetType> Get(const std::string &name)
        {
            std::shared_ptr<AssetType> weakptr = std::get<std::shared_ptr<AssetType>>(assets.at(GetTypeName<AssetType>(name)).asset);
            //return weakptr.lock();
            return weakptr;
        }

        void Cleanup()
        {
            std::for_each(std::begin(assets), std::end(assets), [](auto &value){ value.second.count--; });
            std::experimental::erase_if(assets, [](const auto &value){ return value.second.count <= 0; });
        }

        void Clear()
        {
            assets.clear();
        }

    private:
        template<typename AssetType>
        std::string GetTypeName(const std::string &name) const
        {
            return fmt::format("{}|{}", name, typeid(AssetType).name());
        }

    private:
        std::unordered_map<std::string, RefCount> assets;

    };

    using AssetCache = TAssetCache<Graphics::Font, Graphics::Mesh, Graphics::Model, Graphics::Shader, Graphics::Texture, Graphics::Image, Graphics::Animation::Skeleton, Graphics::Animation::Animation, Graphics::MapMesh, Physics::WorldSolids, std::string>;
}
