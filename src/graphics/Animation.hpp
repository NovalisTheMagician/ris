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
                const Frame<N>& operator[](std::size_t index) const;

                float AdjustTimeToFitTrack(float time, bool looping) const;

            private:
                T SampleConstant(float time, bool looping) const;
                T SampleLinear(float time, bool looping) const;
                T SampleCubic(float time, bool looping) const;
                T Hermite(float t, const T &p1, const T &s1, const T &p2, const T &s2) const;

                T Cast(const float *value) const;

            protected:
                virtual int FrameIndex(float time, bool looping) const;

            protected:
                std::vector<Frame<N>> frames;
                Interpolation interpolation;
                
            };

            using ScalarTrack       = Track<float, 1>;
            using VectorTrack       = Track<glm::vec3, 3>;
            using QuaternionTrack   = Track<glm::quat, 4>;

            template<typename T, unsigned int N>
            class FastTrack : public Track<T, N>
            {
            public:
                void UpdateIndexLookupTable();

            protected:
                int FrameIndex(float time, bool looping) const override;

            private:
                std::vector<std::size_t> sampledFrames;

            };

            using FastScalarTrack       = FastTrack<float, 1>;
            using FastVectorTrack       = FastTrack<glm::vec3, 3>;
            using FastQuaternionTrack   = FastTrack<glm::quat, 4>;

            template<typename T, unsigned int N>
            FastTrack<T, N> OptimizeTrack(const Track<T, N>& input);

            template<typename VTRACK, typename QTRACK>
            class TTransformTrack
            {
            public:
                TTransformTrack();

                unsigned int GetId() const;
                void SetId(unsigned int id);

                VTRACK& GetPositionTrack();
                QTRACK& GetRotationTrack();
                VTRACK& GetScaleTrack();
                const VTRACK& GetPositionTrack() const;
                const QTRACK& GetRotationTrack() const;
                const VTRACK& GetScaleTrack() const;

                float GetStartTime() const;
                float GetEndTime() const;

                bool IsValid() const;

                Transform Sample(const Transform &ref, float time, bool looping) const;

            private:
                unsigned int id;
                VTRACK positionTrack;
                QTRACK rotationTrack;
                VTRACK scaleTrack;

            };

            using TransformTrack = TTransformTrack<VectorTrack, QuaternionTrack>;
            using FastTransformTrack = TTransformTrack<FastVectorTrack, FastQuaternionTrack>;

            FastTransformTrack OptimizeTransformTrack(const TransformTrack &input);

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

            template<typename TRACK>
            class TClip
            {
            public:
                TClip();

                std::size_t GetIdAtIndex(std::size_t index) const;
                void SetIdAtIndex(std::size_t index, std::size_t id);
                std::size_t Size() const;

                float Sample(Pose &outPose, float inTime) const;
                TRACK& operator[](std::size_t index);
                const TRACK& operator[](std::size_t index) const;

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
                std::vector<TRACK> tracks;
                std::string name;
                float startTime;
                float endTime;
                bool looping;

            };

            using Clip = TClip<TransformTrack>;
            using FastClip = TClip<FastTransformTrack>;

            FastClip OptimizeClip(const Clip &input);

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

                Animation(std::vector<FastClip> &&clips);

                FastClip& GetByName(const std::string &clipName);
                const FastClip& GetByName(const std::string &clipName) const;

                FastClip& GetByIndex(std::size_t index);
                const FastClip& GetByIndex(std::size_t index) const;

                FastClip& operator[](const std::string &clipName);
                const FastClip& operator[](const std::string &clipName) const;
                FastClip& operator[](std::size_t index);
                const FastClip& operator[](std::size_t index) const;

            private:
                std::vector<FastClip> clips;
                std::unordered_map<std::string, std::size_t> nameToIndex;

            };
        }
    }
}
