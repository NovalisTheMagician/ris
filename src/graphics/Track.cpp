#include "Graphics/Animation.hpp"

#include "misc/MathHelper.hpp"

#include <cmath>

#include <glm/gtc/type_ptr.hpp>

namespace RIS
{
    namespace Graphics
    {
        namespace Animation
        {
            namespace TrackHelpers
            {
                inline float Interpolate(float a, float b, float t)
                {
                    return a + (b - a) * t;
                }

                inline glm::vec3 Interpolate(const glm::vec3 &a, const glm::vec3 &b, float t)
                {
                    return Lerp(a, b, t);
                }

                inline glm::quat Interpolate(const glm::quat &a, const glm::quat &b, float t)
                {
                    //return glm::lerp(a, b, t);

                    auto result = glm::mix(a, b, t);
                    if(glm::dot(a, b) < 0)
                        result = glm::mix(a, -b, t);
                    return glm::normalize(result);
                }

                inline float AdjustHermiteResult(float f)
                {
                    return f;
                }

                inline glm::vec3 AdjustHermiteResult(const glm::vec3 &v)
                {
                    return v;
                }

                inline glm::quat AdjustHermiteResult(const glm::quat &q)
                {
                    return glm::normalize(q);
                }

                inline void Neighborhood(const float &a, float &b) {}
                inline void Neighborhood(const glm::vec3 &a, glm::vec3 &b) {}
                inline void Neighborhood(const glm::quat &a, glm::quat &b)
                {
                    if(glm::dot(a, b) < 0)
                        b = -b;
                }
            }

            template Track<float, 1>;
            template Track<glm::vec3, 3>;
            template Track<glm::quat, 4>;

            template<typename T, unsigned int N>
            Track<T, N>::Track()
                : interpolation(Interpolation::LINEAR)
            {}

            template<typename T, unsigned int N>
            float Track<T, N>::GetStartTime() const
            {
                return frames.front().time;
            }

            template<typename T, unsigned int N>
            float Track<T, N>::GetEndTime() const
            {
                return frames.back().time;
            }

            template<typename T, unsigned int N>
            T Track<T, N>::Sample(float time, bool looping) const
            {
                switch(interpolation)
                {
                case Interpolation::CONSTANT: return SampleConstant(time, looping);
                case Interpolation::LINEAR: return SampleLinear(time, looping);
                case Interpolation::CUBIC: return SampleCubic(time, looping);
                }
                return T(); // should never reach
            }

            template<typename T, unsigned int N>
            Frame<N>& Track<T, N>::operator[](std::size_t index)
            {
                return frames.at(index);
            }

            template<typename T, unsigned int N>
            void Track<T, N>::Resize(std::size_t size)
            {
                frames.resize(size);
            }

            template<typename T, unsigned int N>
            std::size_t Track<T, N>::Size() const
            {
                return frames.size();
            }

            template<typename T, unsigned int N>
            void Track<T, N>::SetInterpolation(Interpolation interpol)
            {
                interpolation = interpol;
            }

            template<typename T, unsigned int N>
            Interpolation Track<T, N>::GetInterpolation() const
            {
                return interpolation;
            }

            template<typename T, unsigned int N>
            T Track<T, N>::Hermite(float t, const T &p1, const T &s1, const T &_p2, const T &s2) const
            {
                float tt = t * t;
                float ttt = tt * t;

                T p2 = _p2;
                TrackHelpers::Neighborhood(p1, p2);
 
                float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
                float h2 = -2.0f * ttt + 3.0f * tt;
                float h3 = ttt - 2.0f * tt + t;
                float h4 = ttt - tt;

                T result = p1 * h1 + p2 * h2 + s1 * h3 + s2 * h4;
                return TrackHelpers::AdjustHermiteResult(result);
            }

            template<typename T, unsigned int N>
            int Track<T, N>::FrameIndex(float time, bool looping) const
            {
                std::size_t size = frames.size();
                if(size <= 1)
                    return -1;

                if(looping)
                {
                    float startTime = frames.front().time;
                    float endTime = frames.back().time;
                    float duration = endTime - startTime;

                    time = std::fmod(time - startTime, duration);
                    if(time < 0.0f)
                        time += duration;
                    time += startTime;
                }
                else
                {
                    if(time <= frames.front().time)
                        return 0;
                    if(time >= frames.rbegin()[1].time) // second to last element
                        return static_cast<int>(size) - 2;
                }
                
                int index = size - 1;
                for(auto it = frames.rbegin(); it != frames.rend(); ++it)
                {
                    if(time >= (*it).time)
                        return index;
                    index--;
                }
                return -1;
            }

            template<typename T, unsigned int N>
            float Track<T, N>::AdjustTimeToFitTrack(float time, bool looping) const
            {
                std::size_t size = frames.size();
                if(size <= 1)
                    return 0.0f;

                float startTime = frames.front().time;
                float endTime = frames.back().time;
                float duration = endTime - startTime;

                if(looping)
                {
                    time = std::fmod(time - startTime, duration);
                    if(time < 0.0f)
                        time += duration;
                    time += startTime;
                }
                else
                {
                    if(time <= frames.front().time)
                        time = startTime;
                    if(time >= frames.back().time)
                        time = endTime;
                }
                return time;
            }

            template<>
            float Track<float, 1>::Cast(const float *value) const
            {
                return value[0];
            }

            template<>
            glm::vec3 Track<glm::vec3, 3>::Cast(const float *value) const
            {
                return glm::make_vec3(value);
            }

            template<>
            glm::quat Track<glm::quat, 4>::Cast(const float *value) const
            {
                glm::quat q = glm::make_quat(value);
                return glm::normalize(q);
            }

            template<typename T, unsigned int N>
            T Track<T, N>::SampleConstant(float time, bool looping) const
            {
                int frame = FrameIndex(time, looping);
                if(frame < 0 || frame >= frames.size())
                    return T();
                return Cast(frames.at(frame).value.data());
            }

            template<typename T, unsigned int N>
            T Track<T, N>::SampleLinear(float time, bool looping) const
            {
                int thisFrame = FrameIndex(time, looping);
                if(thisFrame < 0 || thisFrame >= frames.size() - 1)
                    return T();

                int nextFrame = thisFrame + 1;

                float trackTime = AdjustTimeToFitTrack(time, looping);
                float thisTime = frames.at(thisFrame).time;
                float frameDelta = frames.at(nextFrame).time - thisTime;

                if(frameDelta <= 0.0f)
                    return T();

                float t = (trackTime - thisTime) / frameDelta;

                T start = Cast(frames.at(thisFrame).value.data());
                T end = Cast(frames.at(nextFrame).value.data());

                return TrackHelpers::Interpolate(start, end, t);
            }

            template<typename T, unsigned int N>
            T Track<T, N>::SampleCubic(float time, bool looping) const
            {
                int thisFrame = FrameIndex(time, looping);
                if(thisFrame < 0 || thisFrame >= frames.size() - 1)
                    return T();

                int nextFrame = thisFrame + 1;

                float trackTime = AdjustTimeToFitTrack(time, looping);
                float thisTime = frames.at(thisFrame).time;
                float frameDelta = frames.at(nextFrame).time - thisTime;

                if(frameDelta <= 0.0f)
                    return T();

                float t = (trackTime - thisTime) / frameDelta;
                std::size_t fltSize = sizeof(float);

                T point1 = Cast(frames.at(thisFrame).value.data());
                T slope1;
                std::memcpy(&slope1, frames.at(thisFrame).out.data(), N * fltSize);
                slope1 *= frameDelta;

                T point2 = Cast(frames.at(nextFrame).value.data());
                T slope2;
                std::memcpy(&slope2, frames.at(nextFrame).in.data(), N * fltSize);
                slope2 *= frameDelta;

                return Hermite(t, point1, slope1, point2, slope2);
            }
        }
    }
}
