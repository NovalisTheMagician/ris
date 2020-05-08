#include "graphics/Model.hpp"

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

using namespace gl46core;

namespace RIS
{
    namespace Graphics
    {
        Model::Model(Mesh &mesh, Texture &texture)
            : mesh(mesh), texture(texture)
        {}

        Model::Model(Mesh &mesh, Texture &texture, std::vector<Joint> &&joints)
            : mesh(mesh), texture(texture), joints(std::move(joints))
        {
            jointTransforms.resize(joints.size());
            for(size_t i = 0; i < joints.size(); ++i)
            {
                jointTransforms[i] = joints.at(i).inverseBindTransform;
            }
        }

        Mesh& Model::GetMesh()
        {
            return mesh;
        }

        Texture& Model::GetTexture()
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
