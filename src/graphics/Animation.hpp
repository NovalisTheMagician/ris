#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <unordered_map>

namespace RIS
{
    struct Joint
    {
    public:
        int index = 0;
        std::string name;
        std::vector<size_t> childrenIndices;

        glm::mat4 animTransform = glm::mat4(1.0f);

        glm::mat4 inverseBindTransform = glm::mat4(1.0f);

        Joint(int index, const std::string &name, const glm::mat4 &inverseBindTransform);
        
        Joint() = default;
        ~Joint() = default;
        Joint(const Joint&) = default;
        Joint& operator=(const Joint&) = default;
        Joint(Joint&&) = default;
        Joint& operator=(Joint&&) = default;

        void AddChild(size_t jointIndex);
    };

    struct JointTransform
    {
        glm::vec3 position;
        glm::vec4 rotation;
    };

    struct KeyFrame
    {
        std::vector<JointTransform> transforms;
        float timeStamp;
    };

    class Animation
    {
        std::vector<KeyFrame> keyFrames;
    };

    class Animator
    {
    public:
        Animator();
        ~Animator();

        Animator(const Animator&) = default;
        Animator& operator=(const Animator&) = default;
        Animator(Animator &&) = default;
        Animator& operator=(Animator&&) = default;

        void AddAnimation(const std::string &name, const Animation &animation);

    private:
        std::unordered_map<std::string, Animation> animations;
        float currentTime = 0.0f;

    };
}
