#include "WinNetwork.hpp"

#ifdef _WIN32

namespace RIS
{
    WinNetwork::WinNetwork(const SystemLocator &systems, Config &config)
        : systems(systems), config(config)
    {
        if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        {
            throw NetworkException("Winsock failed to init");
        }
    }

    WinNetwork::~WinNetwork()
    {
        WSACleanup();
    }

    void WinNetwork::Setup()
    {
        
    }

    void WinNetwork::Connect(const std::string &hostName, unsigned short port)
    {
        //todo
        isServer = false;
    }

    void WinNetwork::Host(bool publicSession)
    {
        //todo
        isServer = true;
    }

    bool WinNetwork::IsServer() const
    {
        return isServer;
    }

    void WinNetwork::StepLoop()
    {
        if(isServer)
        {

        }
        else
        {

        }
    }
}

#endif
