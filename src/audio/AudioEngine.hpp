#pragma once

#include "RisExcept.hpp"

#include <string>

#include <soloud.h>

#include <misc/Timer.hpp>

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

        void Update(const Timer timer);

    private:
        SoLoud::Soloud soloud;

    };
}
