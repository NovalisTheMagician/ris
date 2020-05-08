#pragma once

#include <glm/glm.hpp>

namespace RIS
{
    namespace Graphics
    {
        class Camera
        {
        public:
            Camera();
            ~Camera();

        private:
            glm::mat4 projection;
            glm::mat4 view;

            glm::vec3 position;
            float yaw, pitch;

        };
    }
}
