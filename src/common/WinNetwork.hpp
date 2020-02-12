#pragma once

#include "INetwork.hpp"

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include <atomic>

#ifdef _WIN32

#include <WinSock2.h>
#include <ws2tcpip.h>

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

        void Connect(const std::string &hostName, const std::string &port) override;
        void Host(bool publicSession) override;
        void Disconnect() override;

        bool IsServer() const override;
        bool IsConnecting() const override;
        bool IsConnected() const override;

        void StepLoop() override;

    private:
        bool IsValidServer() const;
        bool IsValidClient() const;

        bool ReadSocket(SOCKET socket);

    private:
        const SystemLocator &systems;
        Config &config;

        std::atomic_bool isServer;

        std::atomic_bool isConnecting;
        std::atomic_bool isConnected;

        SOCKET serverSocket;
        std::vector<SOCKET> clientSockets;

        fd_set *readSet;

        WSADATA wsa;

    };
}

#endif
