#pragma once

#include "INetwork.hpp"

#ifdef __linux__

namespace RIS
{
    class PosixNetwork : public INetwork
    {
        PosixNetwork();
        ~PosixNetwork();

        void Setup() override;
    };
}

#endif
