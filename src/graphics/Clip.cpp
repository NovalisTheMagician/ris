#include "Animation.hpp"

#include <limits>

namespace RIS
{
    namespace Graphics
    {
        namespace Animation
        {
            Clip::Clip()
                : name("No Name"), startTime(0.0f), endTime(0.0f), looping(true)
            {}

            float Clip::Sample(Pose &outPose, float time) const
            {
                if(GetDuration() == 0.0f)
                    return 0.0f;

                time = AdjustTimeToFitRange(time);

                std::size_t size = tracks.size();
                for(auto it = tracks.begin(); it != tracks.end(); ++it)
                {
                    std::size_t id = (*it).GetId();
                    Transform local = outPose.GetLocalTransform(id);
                    Transform animated = (*it).Sample(local, time, looping);
                    outPose.SetLocalTransform(id, animated);
                }
                return time;
            }

            float Clip::AdjustTimeToFitRange(float inTime) const
            {
                if (looping) 
                {
                    float duration = endTime - startTime;
                    if (duration <= 0) { 0.0f; }
                    inTime = std::fmod(inTime - startTime, duration);
                    if (inTime < 0.0f) 
                    {
                        inTime += duration;
                    }
                    inTime = inTime + startTime;
                }
                else 
                {
                    if (inTime < startTime) 
                    { 
                        inTime = startTime;
                    }
                    if (inTime > endTime) 
                    {
                        inTime = endTime;
                    }
                }
                return inTime;
            }

            void Clip::RecalculateDuration()
            {
                startTime = std::numeric_limits<float>::max(); // 0.0f
                endTime = std::numeric_limits<float>::min(); // 0.0f
                //bool startSet = false;
                //bool endSet = false;
                for(auto it = tracks.begin(); it != tracks.end(); ++it)
                {
                    auto &track = *it;
                    if(track.IsValid())
                    {
                        float _startTime = track.GetStartTime();
                        float _endTime = track.GetEndTime();

                        if(_startTime < startTime) //  || !startSet
                        {
                            startTime = _startTime;
                            //startSet = true;
                        }
                        if(_endTime > endTime) // || !endSet
                        {
                            endTime = _endTime;
                            //endSet = true;
                        }
                    }
                }
            }

            TransformTrack& Clip::operator[](std::size_t joint)
            {
                for(auto it = tracks.begin(); it != tracks.end(); ++it)
                {
                    auto &track = *it;
                    if(track.GetId() == joint)
                        return track;
                }

                tracks.emplace_back();
                tracks.back().SetId(joint);
                return tracks.back();
            }

            const std::string& Clip::GetName() const
            {
                return name;
            }

            std::size_t Clip::GetIdAtIndex(std::size_t index) const
            {
                return tracks.at(index).GetId();
            }

            std::size_t Clip::Size() const
            {
                return tracks.size();
            }

            float Clip::GetDuration() const
            {
                return endTime - startTime;
            }

            float Clip::GetStartTime() const
            {
                return startTime;
            }

            float Clip::GetEndTime() const
            {
                return endTime;
            }

            bool Clip::GetLooping() const
            {
                return looping;
            }

            void Clip::SetName(const std::string &newName)
            {
                name = newName;
            }

            void Clip::SetIdAtIndex(std::size_t index, std::size_t id)
            {
                tracks.at(index).SetId(id);
            }

            void Clip::SetLooping(bool newLooping)
            {
                looping = newLooping;
            }
        }
    }
}
