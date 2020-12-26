#pragma once

#include "RisExcept.hpp"

#include <string>

#include <soloud.h>

namespace RIS::Audio
{
    struct AudioException : public RISException
    {
        AudioException(const std::string &reason) : RISException(reason) {}
    };

    class AudioEngine
    {
    public:
        AudioEngine();
        ~AudioEngine();

        AudioEngine(const AudioEngine &) = delete;
        AudioEngine(AudioEngine &&) = delete;
        AudioEngine &operator=(const AudioEngine &) = delete;
        AudioEngine &operator=(AudioEngine &&) = delete;

        void PostInit();

        void PlaySound(const int &soundId);

    private:
        SoLoud::Soloud soloud;

    };
}
