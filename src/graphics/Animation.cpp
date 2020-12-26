#include "graphics/Animation.hpp"

namespace RIS::Graphics::Animation
{
    Animation::Animation(std::vector<FastClip> &&newClips)
    {
        clips = std::move(newClips);
        for(std::size_t i = 0; i < clips.size(); ++i)
        {
            auto &clip = clips.at(i);
            nameToIndex.insert_or_assign(clip.GetName(), i);
        }
    }

    FastClip& Animation::GetByName(const std::string &clipName)
    {
        return clips.at(nameToIndex.at(clipName));
    }

    const FastClip& Animation::GetByName(const std::string &clipName) const
    {
        return clips.at(nameToIndex.at(clipName));
    }

    FastClip& Animation::GetByIndex(std::size_t index)
    {
        return clips.at(index);
    }

    const FastClip& Animation::GetByIndex(std::size_t index) const
    {
        return clips.at(index);
    }

    FastClip& Animation::operator[](const std::string &clipName)
    {
        return GetByName(clipName);
    }

    const FastClip& Animation::operator[](const std::string &clipName) const
    {
        return GetByName(clipName);
    }

    FastClip& Animation::operator[](std::size_t index)
    {
        return GetByIndex(index);
    }

    const FastClip& Animation::operator[](std::size_t index) const
    {
        return GetByIndex(index);
    }
}
