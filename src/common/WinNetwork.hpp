#pragma once

#include "INetwork.hpp"

#include "common/SystemLocator.hpp"

#ifdef _WIN32

namespace RIS
{
    class WinNetwork : public INetwork
    {
    public:
        WinNetwork(const SystemLocator &systems);
        ~WinNetwork();

        void Setup() override;

    private:
        const SystemLocator &systems;

    };
}

#endif
