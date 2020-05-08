#pragma once

#include <string>
#include <stdexcept>

#include <soloud.h>

namespace RIS
{
    namespace Audio
    {
        struct AudioException : public std::runtime_error
        {
            AudioException(std::string reason) : std::runtime_error(reason.c_str()) {}
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
}
