#pragma once

#include <string>

namespace RIS
{
    class INetwork
    {
    public:
        virtual ~INetwork() = default;
        virtual void Setup() = 0;

        virtual void Connect(const std::string &hostName, const std::string &port) = 0;
        virtual void Host(bool publicSession) = 0;
        virtual void Disconnect() = 0;

        virtual bool IsServer() const = 0;
        virtual bool IsConnecting() const = 0;
        virtual bool IsConnected() const = 0;

        virtual void StepLoop() = 0;
    };
}