#pragma once

#include "common/IRenderer.hpp"

#include "Mesh.hpp"
#include "Animation.hpp"
#include "Texture.hpp"

#include <vector>

namespace RIS
{
    class Model
    {
    public:
        Model(ResourceId meshId, ResourceId textureId);
        Model(ResourceId meshId, ResourceId textureId, std::vector<Joint> &&joints);

        ~Model() = default;
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;
        Model(Model&&) = default;
        Model& operator=(Model&&) = default;

        ResourceId GetMesh() const;
        ResourceId GetTexture() const;

        bool IsAnimated() const;
        Animator& GetAnimator();

        std::vector<glm::mat4>& GetJointTransforms();

    private:
        ResourceId meshId;
        ResourceId textureId;

        Animator animator;
        std::vector<Joint> joints;
        std::vector<glm::mat4> jointTransforms;

    };
}
