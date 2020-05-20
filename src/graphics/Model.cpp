#include "graphics/Model.hpp"

namespace RIS
{
    namespace Graphics
    {
        Model::Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture)
            : mesh(mesh), texture(texture)
        {}

        Model::Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, std::vector<Joint> &&joints)
            : mesh(mesh), texture(texture), joints(std::move(joints))
        {
            jointTransforms.resize(joints.size());
            for(size_t i = 0; i < joints.size(); ++i)
            {
                jointTransforms[i] = joints.at(i).inverseBindTransform;
            }
        }

        std::shared_ptr<Mesh> Model::GetMesh()
        {
            return mesh;
        }

        std::shared_ptr<Texture> Model::GetTexture()
        {
            return texture;
        }

        bool Model::IsAnimated() const
        {
            return joints.size() != 0;
        }

        Animator& Model::GetAnimator()
        {
            return animator;
        }

        std::vector<glm::mat4>& Model::GetJointTransforms()
        {
            return jointTransforms;
        }
    }
}
