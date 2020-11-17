#pragma once

#include "graphics/Mesh.hpp"
#include "graphics/Animation.hpp"
#include "graphics/Texture.hpp"

#include <vector>
#include <memory>

#include "loader/LoadFunc.hpp"

namespace RIS
{
    namespace Graphics
    {
        class Model
        {
        public:
            using Ptr = std::shared_ptr<Model>;

            Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture);

            ~Model() = default;
            Model(const Model&) = delete;
            Model& operator=(const Model&) = delete;
            Model(Model&&) = default;
            Model& operator=(Model&&) = default;

            std::shared_ptr<Mesh> GetMesh();
            std::shared_ptr<Texture> GetTexture();

        private:
            std::shared_ptr<Mesh> mesh;
            std::shared_ptr<Texture> texture;

        };
    }

    namespace Loader
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
}
