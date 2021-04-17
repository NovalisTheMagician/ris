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

        Model(Mesh::Ptr mesh, Texture::Ptr texture);

        ~Model() = default;
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;
        Model(Model&&) = default;
        Model& operator=(Model&&) = default;

        Mesh::Ptr GetMesh();
        Texture::Ptr GetTexture();

        void Bind(VertexArray &vao) const;
        void Draw() const;

    private:
        Mesh::Ptr mesh;
        Texture::Ptr texture;

    };
}
