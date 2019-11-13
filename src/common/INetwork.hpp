#pragma once

namespace RIS
{
    class INetwork
    {
    public:
        virtual ~INetwork() = default;
        virtual void Setup() = 0;
    };
}