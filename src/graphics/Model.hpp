#pragma once

#include "graphics/Mesh.hpp"
#include "graphics/Animation.hpp"
#include "graphics/Texture.hpp"

#include <vector>
#include <memory>

namespace RIS
{
    namespace Graphics
    {
        class Model
        {
        public:
            Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture);
            Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, std::vector<Joint> &&joints);

            ~Model() = default;
            Model(const Model&) = delete;
            Model& operator=(const Model&) = delete;
            Model(Model&&) = default;
            Model& operator=(Model&&) = default;

            std::shared_ptr<Mesh> GetMesh();
            std::shared_ptr<Texture> GetTexture();

            bool IsAnimated() const;
            Animator& GetAnimator();

            std::vector<glm::mat4>& GetJointTransforms();

        private:
            std::shared_ptr<Mesh> mesh;
            std::shared_ptr<Texture> texture;

            Animator animator;
            std::vector<Joint> joints;
            std::vector<glm::mat4> jointTransforms;

        };
    }
}
