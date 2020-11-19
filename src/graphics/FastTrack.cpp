#include "graphics/Animation.hpp"

#include <cmath>

namespace RIS
{
    namespace Graphics
    {
        namespace Animation
        {
            template FastTrack<float, 1>;
            template FastTrack<glm::vec3, 3>;
            template FastTrack<glm::quat, 4>;

            template<typename T, unsigned int N>
            void FastTrack<T, N>::UpdateIndexLookupTable()
            {
                std::size_t numFrames = frames.size();
                if(numFrames <= 1)
                    return;

                float duration = GetEndTime() - GetStartTime();
                std::size_t numSamples = static_cast<std::size_t>(duration * SAMPLES_PER_SECOND);
                sampledFrames.resize(numSamples);

                for(std::size_t i = 0; i < numSamples; ++i)
                {
                    float t = static_cast<float>(i) / static_cast<float>(numSamples - 1);
                    float time = t * duration + GetStartTime();

                    std::size_t frameIndex = 0;
                    for(std::size_t j = numFrames - 1; j >= 0; --j)
                    {
                        if(time >= frames.at(j).time)
                        {
                            frameIndex = j;
                            if(frameIndex >= numFrames - 2)
                                frameIndex = numFrames - 2;
                            break;
                        }
                    }
                    sampledFrames[i] = frameIndex;
                }
            }

            template<typename T, unsigned int N>
            int FastTrack<T, N>::FrameIndex(float time, bool looping) const
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
                    if(time < 0)
                        time += duration;
                    //time += startTime;
                }
                else
                {
                    if(time <= frames.front().time)
                        return 0;
                    if(time >= frames.rbegin()[1].time) // second to last element
                        return static_cast<int>(size) - 2;
                }
                
                float duration = GetEndTime() - GetStartTime();
                float t = time / duration;

                std::size_t numSamples = static_cast<std::size_t>(duration * SAMPLES_PER_SECOND);
                std::size_t index = static_cast<std::size_t>(t * numSamples);
                if(index >= sampledFrames.size())
                    return -1;
                return sampledFrames.at(index);
            }

            template FastTrack<float, 1>        OptimizeTrack(const Track<float, 1> &input);
            template FastTrack<glm::vec3, 3>    OptimizeTrack(const Track<glm::vec3, 3> &input);
            template FastTrack<glm::quat, 4>    OptimizeTrack(const Track<glm::quat, 4> &input);

            template<typename T, unsigned int N>
            FastTrack<T, N> OptimizeTrack(const Track<T, N> &input)
            {
                FastTrack<T, N> result;
                result.SetInterpolation(input.GetInterpolation());
                std::size_t size = input.Size();
                result.Resize(size);
                for(std::size_t i = 0; i < size; ++i)
                    result[i] = input[i];
                result.UpdateIndexLookupTable();
                return result;
            }
        }
    }
}
