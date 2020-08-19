#include "graphics/Animation.hpp"

namespace RIS
{
    namespace Graphics
    {
        namespace Animation
        {
            Pose::Pose(std::size_t numJoints)
                : joints(numJoints), parents(numJoints, -1)
            {}

            void Pose::Resize(std::size_t size)
            {
                parents.resize(size, -1);
                joints.resize(size);
            }

            std::size_t Pose::Size() const
            {
                return joints.size();
            }

            Transform Pose::GetLocalTransform(std::size_t index) const
            {
                return joints.at(index);
            }

            void Pose::SetLocalTransform(std::size_t index, const Transform &transform)
            {
                joints[index] = transform;
            }

            Transform Pose::GetGlobalTransform(std::size_t index) const
            {
                Transform result = joints.at(index);
                for(int p = parents.at(index); p >= 0; p = parents.at(p))
                {
                    result = Combine(joints.at(p), result);
                }

                return result;
            }

            Transform Pose::operator[](std::size_t index) const
            {
                return GetGlobalTransform(index);
            }

            void Pose::GetMatrixPalette(std::vector<glm::mat4> &out) const
            {
                std::size_t size = Size();
                if(out.size() != size)
                    out.resize(size);
                
                for(std::size_t i = 0; i < size; ++i)
                {
                    Transform t = GetGlobalTransform(i);
                    out[i] = TransformToMat4(t);
                }
            }

            int Pose::GetParent(std::size_t index) const
            {
                return parents.at(index);
            }

            void Pose::SetParent(std::size_t index, int parent)
            {
                parents[index] = parent;
            }

            bool Pose::operator==(const Pose &other) const
            {
                if(joints.size() != other.joints.size()) return false;
                if(parents.size() != other.parents.size()) return false;

                std::size_t size = joints.size();
                for(std::size_t i = 0; i < size; ++i)
                {
                    Transform thisLocal = joints.at(i);
                    Transform otherLocal = other.joints.at(i);
                    int thisParent = parents.at(i);
                    int otherParent = other.parents.at(i);

                    if(thisParent != otherParent) return false;
                    if(thisLocal.position != otherLocal.position) return false;
                    if(thisLocal.rotation != otherLocal.rotation) return false;
                    if(thisLocal.scale != otherLocal.scale) return false;
                }
                return true;
            }

            bool Pose::operator!=(const Pose &other) const
            {
                return !(*this == other);
            }
        }
    }
}
