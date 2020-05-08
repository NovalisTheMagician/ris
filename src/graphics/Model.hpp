#pragma once

#include "graphics/Mesh.hpp"
#include "graphics/Animation.hpp"
#include "graphics/Texture.hpp"

#include <vector>

namespace RIS
{
    namespace Graphics
    {
        class Model
        {
        public:
            Model(Mesh &mesh, Texture &texture);
            Model(Mesh &mesh, Texture &texture, std::vector<Joint> &&joints);

            ~Model() = default;
            Model(const Model&) = delete;
            Model& operator=(const Model&) = delete;
            Model(Model&&) = default;
            Model& operator=(Model&&) = default;

            Mesh& GetMesh();
            Texture& GetTexture();

            bool IsAnimated() const;
            Animator& GetAnimator();

            std::vector<glm::mat4>& GetJointTransforms();

        private:
            Mesh &mesh;
            Texture &texture;

            Animator animator;
            std::vector<Joint> joints;
            std::vector<glm::mat4> jointTransforms;

        };
    }
}
