#pragma once

#include <string>

namespace RIS
{
    class IAudio
    {
    public:
        virtual ~IAudio() = default;

        virtual int LoadSound(const std::string &soundName) = 0;
        virtual void PlaySound(const int &soundId) = 0;
    };
}
