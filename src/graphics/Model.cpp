#include "graphics/Model.hpp"

namespace RIS::Graphics
{
    Model::Model(Mesh::Ptr mesh, Texture::Ptr texture)
        : mesh(mesh), texture(texture)
    {}

    Mesh::Ptr Model::GetMesh()
    {
        return mesh;
    }

    Texture::Ptr Model::GetTexture()
    {
        return texture;
    }

    void Model::Bind(VertexArray &vao) const
    {
        mesh->Bind(vao);
        texture->Bind(0);
    }

    void Model::Draw() const
    {
        mesh->Draw();
    }
}
