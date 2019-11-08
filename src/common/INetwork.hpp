#pragma once

namespace RIS
{
    class INetwork
    {
    public:
        virtual ~INetwork() {};
        virtual void Setup() = 0;
    };
}