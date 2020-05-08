#include "audio/AudioEngine.hpp"

#include "loader/Loader.hpp"

namespace RIS
{
    namespace Audio
    {
        AudioEngine::AudioEngine()
        {
            SoLoud::result res = soloud.init();
            if(res != 0)
                throw AudioException("Couldn't initialize Soloud");
        }

        AudioEngine::~AudioEngine()
        {
            soloud.deinit();
        }

        void AudioEngine::PostInit()
        {

        }

        void AudioEngine::PlaySound(const int &soundId)
        {

        }
    }
}
