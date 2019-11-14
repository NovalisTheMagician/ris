#include "SOLAudio.hpp"

using std::string;

namespace RIS
{
    SOLAudio::SOLAudio(const SystemLocator &systems)
        : systems(systems)
    {
        SoLoud::result res = soloud.init();
        if(res != 0)
            throw AudioException("Couldn't initialize Soloud");
    }

    SOLAudio::~SOLAudio()
    {
        soloud.deinit();
    }

    int SOLAudio::LoadSound(const string &soundName)
    {
        return 0;
    }

    void SOLAudio::PlaySound(const int &soundId)
    {

    }
}
