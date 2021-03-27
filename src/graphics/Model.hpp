#pragma once

#include "graphics/Mesh.hpp"
#include "graphics/Animation.hpp"
#include "graphics/Texture.hpp"
#include "graphics/VertexArray.hpp"

#include <memory>

namespace RIS::Graphics
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

        void Bind(VertexArray &vao) const;
        void Draw() const;

    private:
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Texture> texture;

    };
}
