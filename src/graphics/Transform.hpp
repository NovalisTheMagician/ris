#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace RIS::Graphics
{
    struct Transform
    {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        Transform(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
            : position(position), rotation(rotation), scale(scale)
        {}

        Transform()
            : position(0, 0, 0), rotation(1, 0, 0, 0), scale(1, 1, 1)
        {}
    };

    Transform Combine(const Transform &a, const Transform &b);
    Transform Inverse(const Transform &t);
    Transform Mix(const Transform &a, const Transform &b, float t);
    glm::mat4 TransformToMat4(const Transform &t);
    Transform Mat4ToTransform(const glm::mat4 &m);
    glm::vec3 TransformPoint(const Transform &t, const glm::vec3 &p);
    glm::vec3 TransformVector(const Transform &t, const glm::vec3 &v);
}
