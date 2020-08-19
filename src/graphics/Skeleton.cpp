#include "graphics/Animation.hpp"

namespace RIS
{
    namespace Graphics
    {
        namespace Animation
        {
            Skeleton::Skeleton(Pose &&restPose, Pose &&bindPose, std::vector<std::string> &&names)
                : restPose(std::move(restPose)), bindPose(std::move(bindPose)), jointNames(std::move(names))
            {
                UpdateInverseBindPose();
            }

            void Skeleton::Set(Pose &&restPose, Pose &&bindPose, std::vector<std::string> &&names)
            {
                this->restPose = std::move(restPose);
                this->bindPose = std::move(bindPose);
                this->jointNames = std::move(names);
                UpdateInverseBindPose();
            }

            void Skeleton::UpdateInverseBindPose()
            {
                std::size_t size = bindPose.Size();
                invBindPose.resize(size);

                for(std::size_t i = 0; i < size; ++i)
                {
                    Transform world = bindPose.GetGlobalTransform(i);
                    invBindPose[i] = glm::inverse(TransformToMat4(world));
                }
            }

            Pose& Skeleton::GetBindPose()
            {
                return bindPose;
            }

            const Pose& Skeleton::GetBindPose() const
            {
                return bindPose;
            }

            Pose& Skeleton::GetRestPose()
            {
                return restPose;
            }

            const Pose& Skeleton::GetRestPose() const
            {
                return restPose;
            }

            const std::vector<glm::mat4>& Skeleton::GetInvBindPose() const
            {
                return invBindPose;
            }

            const std::vector<std::string>& Skeleton::GetJointNames() const
            {
                return jointNames;
            }

            const std::string& Skeleton::GetJointName(std::size_t index) const
            {
                return jointNames.at(index);
            }
        }
    }
}
