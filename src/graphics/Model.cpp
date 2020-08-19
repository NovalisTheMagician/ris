#include "graphics/Model.hpp"

namespace RIS
{
    namespace Graphics
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
    }
}
