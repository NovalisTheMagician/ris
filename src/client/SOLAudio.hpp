#pragma once

#include "common/IAudio.hpp"
#include "common/SystemLocator.hpp"

#include <string>
#include <stdexcept>

#include <soloud.h>

namespace RIS
{
    struct AudioException : public std::runtime_error
	{
		AudioException(std::string reason) : std::runtime_error(reason.c_str()) {}
	};

    class SOLAudio : public IAudio
    {
    public:
        SOLAudio(const SystemLocator &systems);
        ~SOLAudio();

        SOLAudio(const SOLAudio &) = delete;
        SOLAudio(SOLAudio &&) = delete;
        SOLAudio &operator=(const SOLAudio &) = delete;
        SOLAudio &operator=(SOLAudio &&) = delete;

        int LoadSound(const std::string &soundName) override;
        void PlaySound(const int &soundId) override;

    private:
        const SystemLocator &systems;
        SoLoud::Soloud soloud;

    };
}
