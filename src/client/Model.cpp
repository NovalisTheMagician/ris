#include "Model.hpp"

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

using namespace gl46core;

namespace RIS
{
    Model::Model(ResourceId meshId, ResourceId textureId)
        : meshId(meshId), textureId(textureId)
    {

    }

    Model::Model(ResourceId meshId, ResourceId textureId, std::vector<Joint> &&joints)
        : meshId(meshId), textureId(textureId), joints(std::move(joints))
    {
        jointTransforms.resize(joints.size());
        for(size_t i = 0; i < joints.size(); ++i)
        {
            jointTransforms[i] = joints.at(i).inverseBindTransform;
        }
    }

    ResourceId Model::GetMesh() const
    {
        return meshId;
    }

    ResourceId Model::GetTexture() const
    {
        return textureId;
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
