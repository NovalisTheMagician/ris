#pragma once

#include "loader/LoadFunc.hpp"

#include <memory>

namespace RIS::Graphics
{
    class Model;
    class Mesh;
    namespace Animation
    {
        class Animation;
        class Skeleton;
    }
}

namespace RIS::Loader
{
    template<>
    std::shared_ptr<Graphics::Mesh> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack);

    template<>
    std::shared_ptr<Graphics::Animation::Skeleton> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack);

    template<>
    std::shared_ptr<Graphics::Animation::Animation> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack);

    template<>
    std::shared_ptr<Graphics::Model> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack);
}
