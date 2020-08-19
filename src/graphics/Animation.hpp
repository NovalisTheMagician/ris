#pragma once

#include "graphics/Transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <array>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace RIS
{
    namespace Graphics
    {
        namespace Animation
        {
            enum class Interpolation
            {
                CONSTANT,
                LINEAR,
                CUBIC
            };

            template<unsigned int N>
            struct Frame
            {
                std::array<float, N> value;
                std::array<float, N> in;
                std::array<float, N> out;
                float time;
            };

            using ScalarFrame       = Frame<1>;
            using VectorFrame       = Frame<3>;
            using QuaternionFrame   = Frame<4>;

            template<typename T, unsigned int N>
            class Track
            {
            public:
                Track();

                void Resize(std::size_t size);
                std::size_t Size() const;
                Interpolation GetInterpolation() const;
                void SetInterpolation(Interpolation interp);
                float GetStartTime() const;
                float GetEndTime() const;

                T Sample(float time, bool looping) const;
                Frame<N>& operator[](std::size_t index);

                float AdjustTimeToFitTrack(float time, bool looping) const;

            private:
                T SampleConstant(float time, bool looping) const;
                T SampleLinear(float time, bool looping) const;
                T SampleCubic(float time, bool looping) const;
                T Hermite(float t, const T &p1, const T &s1, const T &p2, const T &s2) const;

                int FrameIndex(float time, bool looping) const;

                T Cast(const float *value) const;

            private:
                std::vector<Frame<N>> frames;
                Interpolation interpolation;
                
            };

            using ScalarTrack       = Track<float, 1>;
            using VectorTrack       = Track<glm::vec3, 3>;
            using QuaternionTrack   = Track<glm::quat, 4>;

            class TransformTrack
            {
            public:
                TransformTrack();

                unsigned int GetId() const;
                void SetId(unsigned int id);

                VectorTrack& GetPositionTrack();
                QuaternionTrack& GetRotationTrack();
                VectorTrack& GetScaleTrack();
                const VectorTrack& GetPositionTrack() const;
                const QuaternionTrack& GetRotationTrack() const;
                const VectorTrack& GetScaleTrack() const;

                float GetStartTime() const;
                float GetEndTime() const;

                bool IsValid() const;

                Transform Sample(const Transform &ref, float time, bool looping) const;

            private:
                unsigned int id;
                VectorTrack positionTrack;
                QuaternionTrack rotationTrack;
                VectorTrack scaleTrack;

            };

            class Pose
            {
            public:
                Pose(std::size_t numJoints);

                Pose() = default;
                ~Pose() = default;
                Pose(const Pose &p) = default;
                Pose& operator=(const Pose &p) = default;
                Pose(Pose &&p) = default;
                Pose& operator=(Pose &&p) = default;

                void Resize(std::size_t size);
                std::size_t Size() const;
                int GetParent(std::size_t index) const;
                void SetParent(std::size_t index, int parent);

                Transform GetLocalTransform(std::size_t index) const;
                void SetLocalTransform(std::size_t index, const Transform &transform);
                Transform GetGlobalTransform(std::size_t index) const;
                Transform operator[](std::size_t index) const;

                void GetMatrixPalette(std::vector<glm::mat4> &out) const;

                bool operator==(const Pose &other) const;
                bool operator!=(const Pose &other) const;

            private:
                std::vector<Transform> joints;
                std::vector<int> parents;

            };

            class Clip
            {
            public:
                Clip();

                std::size_t GetIdAtIndex(std::size_t index) const;
                void SetIdAtIndex(std::size_t index, std::size_t id);
                std::size_t Size() const;

                float Sample(Pose &outPose, float inTime) const;
                TransformTrack& operator[](std::size_t index);
                //const TransformTrack& operator[](std::size_t index) const;

                void RecalculateDuration();

                const std::string& GetName() const;
                void SetName(const std::string &newName);
                float GetDuration() const;
                float GetStartTime() const;
                float GetEndTime() const;
                bool GetLooping() const;
                void SetLooping(bool newLooping);

            private:
                float AdjustTimeToFitRange(float inTime) const;

            private:
                std::vector<TransformTrack> tracks;
                std::string name;
                float startTime;
                float endTime;
                bool looping;

            };

            class Skeleton
            {
            public:
                using Ptr = std::shared_ptr<Skeleton>;

                Skeleton() = default;
                Skeleton(Pose &&restPose, Pose &&bindPose, std::vector<std::string> &&names);

                void Set(Pose &&restPose, Pose &&bindPose, std::vector<std::string> &&names);

                Pose& GetBindPose();
                const Pose& GetBindPose() const;
                Pose& GetRestPose();
                const Pose& GetRestPose() const;

                const std::vector<glm::mat4>& GetInvBindPose() const;
                const std::vector<std::string>& GetJointNames() const;
                const std::string& GetJointName(std::size_t index) const;

            private:
                void UpdateInverseBindPose();

            private:
                Pose restPose;
                Pose bindPose;
                std::vector<glm::mat4> invBindPose;
                std::vector<std::string> jointNames;

            };

            class Animation
            {
            public:
                using Ptr = std::shared_ptr<Animation>;

                Animation(std::vector<Clip> &&clips);

                Clip& GetByName(const std::string &clipName);
                const Clip& GetByName(const std::string &clipName) const;

                Clip& GetByIndex(std::size_t index);
                const Clip& GetByIndex(std::size_t index) const;

                Clip& operator[](const std::string &clipName);
                const Clip& operator[](const std::string &clipName) const;
                Clip& operator[](std::size_t index);
                const Clip& operator[](std::size_t index) const;

            private:
                std::vector<Clip> clips;
                std::unordered_map<std::string, std::size_t> nameToIndex;

            };
        }
    }
}
