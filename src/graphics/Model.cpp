#include "graphics/Model.hpp"

namespace RIS::Graphics
{
    Model::Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture)
        : mesh(mesh), texture(texture)
    {}

    std::shared_ptr<Mesh> Model::GetMesh()
    {
        return mesh;
    }

    std::shared_ptr<Texture> Model::GetTexture()
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
