#include "Animation.hpp"

#include <algorithm>

namespace RIS
{
    Joint::Joint(int index, const std::string &name, const glm::mat4 &inverseBindTransform)
        : index(index), name(name), inverseBindTransform(inverseBindTransform)
    {
    }

    void Joint::AddChild(size_t jointIndex)
    {
        childrenIndices.push_back(jointIndex);
    }

    Animator::Animator()
    {

    }

    Animator::~Animator()
    {

    }
}
