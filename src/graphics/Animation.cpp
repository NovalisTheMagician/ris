#include "graphics/Animation.hpp"

namespace RIS
{
    namespace Graphics
    {
        namespace Animation
        {
            Animation::Animation(std::vector<Clip> &&newClips)
            {
                clips = std::move(newClips);
                for(std::size_t i = 0; i < clips.size(); ++i)
                {
                    auto &clip = clips.at(i);
                    nameToIndex.insert_or_assign(clip.GetName(), i);
                }
            }

            Clip& Animation::GetByName(const std::string &clipName)
            {
                return clips.at(nameToIndex.at(clipName));
            }

            const Clip& Animation::GetByName(const std::string &clipName) const
            {
                return clips.at(nameToIndex.at(clipName));
            }

            Clip& Animation::GetByIndex(std::size_t index)
            {
                return clips.at(index);
            }

            const Clip& Animation::GetByIndex(std::size_t index) const
            {
                return clips.at(index);
            }

            Clip& Animation::operator[](const std::string &clipName)
            {
                return GetByName(clipName);
            }

            const Clip& Animation::operator[](const std::string &clipName) const
            {
                return GetByName(clipName);
            }

            Clip& Animation::operator[](std::size_t index)
            {
                return GetByIndex(index);
            }

            const Clip& Animation::operator[](std::size_t index) const
            {
                return GetByIndex(index);
            }
        }
    }
}
