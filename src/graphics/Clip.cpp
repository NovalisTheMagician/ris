#include "Animation.hpp"

#include <limits>

#include <stdexcept>

namespace RIS::Graphics::Animation
{
    template class TClip<TransformTrack>;
    template class TClip<FastTransformTrack>;

    template<typename TRACK>
    TClip<TRACK>::TClip()
        : name("No Name"), startTime(0.0f), endTime(0.0f), looping(true)
    {}

    template<typename TRACK>
    float TClip<TRACK>::Sample(Pose &outPose, float time) const
    {
        if(GetDuration() == 0.0f)
            return 0.0f;

        time = AdjustTimeToFitRange(time);

        for(auto it = tracks.begin(); it != tracks.end(); ++it)
        {
            std::size_t id = (*it).GetId();
            Transform local = outPose.GetLocalTransform(id);
            Transform animated = (*it).Sample(local, time, looping);
            outPose.SetLocalTransform(id, animated);
        }
        return time;
    }

    template<typename TRACK>
    float TClip<TRACK>::AdjustTimeToFitRange(float inTime) const
    {
        if (looping) 
        {
            float duration = endTime - startTime;
            if (duration <= 0) { return 0.0f; }
            inTime = std::fmod(inTime - startTime, duration);
            if (inTime < 0.0f) 
            {
                inTime += duration;
            }
            inTime += startTime;
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

    template<typename TRACK>
    void TClip<TRACK>::RecalculateDuration()
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

    template<typename TRACK>
    TRACK& TClip<TRACK>::operator[](std::size_t joint)
    {
        for(auto it = tracks.begin(); it != tracks.end(); ++it)
        {
            auto &track = *it;
            if(track.GetId() == joint)
                return track;
        }

        tracks.emplace_back();
        tracks.back().SetId(static_cast<unsigned int>(joint));
        return tracks.back();
    }

    template<typename TRACK>
    const TRACK& TClip<TRACK>::operator[](std::size_t joint) const
    {
        for(auto it = tracks.begin(); it != tracks.end(); ++it)
        {
            auto &track = *it;
            if(track.GetId() == joint)
                return track;
        }

        throw std::runtime_error("ERROR"); // do better
    }

    template<typename TRACK>
    const std::string& TClip<TRACK>::GetName() const
    {
        return name;
    }

    template<typename TRACK>
    std::size_t TClip<TRACK>::GetIdAtIndex(std::size_t index) const
    {
        return tracks.at(index).GetId();
    }

    template<typename TRACK>
    std::size_t TClip<TRACK>::Size() const
    {
        return tracks.size();
    }

    template<typename TRACK>
    float TClip<TRACK>::GetDuration() const
    {
        return endTime - startTime;
    }

    template<typename TRACK>
    float TClip<TRACK>::GetStartTime() const
    {
        return startTime;
    }

    template<typename TRACK>
    float TClip<TRACK>::GetEndTime() const
    {
        return endTime;
    }

    template<typename TRACK>
    bool TClip<TRACK>::GetLooping() const
    {
        return looping;
    }

    template<typename TRACK>
    void TClip<TRACK>::SetName(const std::string &newName)
    {
        name = newName;
    }

    template<typename TRACK>
    void TClip<TRACK>::SetIdAtIndex(std::size_t index, std::size_t id)
    {
        tracks.at(index).SetId(static_cast<unsigned int>(id));
    }

    template<typename TRACK>
    void TClip<TRACK>::SetLooping(bool newLooping)
    {
        looping = newLooping;
    }

    FastClip OptimizeClip(const Clip &input)
    {
        FastClip result;
        result.SetName(input.GetName());
        result.SetLooping(input.GetLooping());
        std::size_t size = input.Size();
        for(std::size_t i = 0; i < size; ++i)
        {
            std::size_t joint = input.GetIdAtIndex(i);
            result[joint] = OptimizeTransformTrack(input[joint]);
        }
        result.RecalculateDuration();
        return result;
    }
}
