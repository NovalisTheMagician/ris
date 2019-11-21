#pragma once

#include "INetwork.hpp"

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#ifdef _WIN32

namespace RIS
{
    class WinNetwork : public INetwork
    {
    public:
        WinNetwork(const SystemLocator &systems, Config &config);
        ~WinNetwork();

        void Setup() override;

    private:
        const SystemLocator &systems;
        Config &config;

    };
}

#endif
