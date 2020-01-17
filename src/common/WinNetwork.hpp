#pragma once

#include "INetwork.hpp"

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#ifdef _WIN32

#include <WinSock2.h>

namespace RIS
{
    struct NetworkException : public std::runtime_error
	{
		NetworkException(std::string reason) : std::runtime_error(reason.c_str()) {}
	};

    class WinNetwork : public INetwork
    {
    public:
        WinNetwork(const SystemLocator &systems, Config &config);
        ~WinNetwork();

        void Setup() override;

        void Connect(const std::string &hostName, unsigned short port) override;
        void Host(bool publicSession) override;

        bool IsServer() const override;

        void StepLoop() override;

    private:
        const SystemLocator &systems;
        Config &config;

        bool isServer;

        WSADATA wsa;

    };
}

#endif
