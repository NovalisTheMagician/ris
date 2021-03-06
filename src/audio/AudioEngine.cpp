#include "audio/AudioEngine.hpp"

#include "RIS.hpp"

#include "loader/Loader.hpp"

namespace RIS::Audio
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

    void AudioEngine::Update(const Timer timer)
    {
        soloud.update3dAudio();
    }
}
