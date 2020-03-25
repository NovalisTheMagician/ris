#include "WinNetwork.hpp"

#include <algorithm>
#include <thread>

#ifdef _WIN32

namespace RIS
{
    WinNetwork::WinNetwork(const SystemLocator &systems, Config &config)
        : systems(systems), config(config), isConnecting(false), isConnected(false), isServer(false)
    {
        if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        {
            throw NetworkException("Winsock failed to init");
        }
    }

    WinNetwork::~WinNetwork()
    {
        Disconnect();

        WSACleanup();
    }

    void WinNetwork::Setup()
    {
        
    }

    void WinNetwork::Connect(const std::string &hostName, const std::string &port)
    {
        if(IsValidServer() || IsValidClient())
        {
            throw NetworkException("Already connected");
        }

        bool forceIPV6 = config.GetValue("net_forceipv6", false);

        struct addrinfo hints = {0};
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = forceIPV6 ? AF_INET6 : AF_UNSPEC;

        struct addrinfo *result;
        if(getaddrinfo(hostName.c_str(), port.c_str(), &hints, &result) != 0)
        {
            throw NetworkException("Failed to to get address info of the server");
        }

        SOCKET clientSocket;
        clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if(clientSocket == INVALID_SOCKET)
        {
            throw NetworkException("Failed to create client socket");
        }

        std::thread connectThread([this, clientSocket, result](){
            isConnecting = true;
            if(connect(clientSocket, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR)
            {
                isConnected = false;
            }
            else
            {
                isConnected = true;
            }
            isConnecting = false;

            freeaddrinfo(result);
        });
        connectThread.detach();

        clientSockets.insert(clientSockets.begin(), clientSocket);
        isServer = false;
    }

    void WinNetwork::Host(bool publicSession)
    {
        if(IsValidServer() || IsValidClient())
        {
            throw NetworkException("Already connected");
        }

        bool forceIPV6 = config.GetValue("net_forceipv6", false);
        std::string port = config.GetValue<std::string>("net_port", "20201");

        struct addrinfo hints = {0};
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = forceIPV6 ? AF_INET6 : AF_UNSPEC;
        hints.ai_flags = AI_PASSIVE;

        struct addrinfo *result;
        if(getaddrinfo(nullptr, port.c_str(), &hints, &result) != 0)
        {
            throw NetworkException("Failed to to get address info of the server");
        }

        serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if(serverSocket == INVALID_SOCKET)
        {
            throw NetworkException("Failed to create server socket");
        }

        if(bind(serverSocket, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR)
        {
            throw NetworkException("Failed to bind server socket to port " + port);
        }

        freeaddrinfo(result);

        if(listen(serverSocket, 5) == SOCKET_ERROR)
        {
            throw NetworkException("Failed to listen on the server socket");
        }

        FD_ZERO(readSet);
        FD_SET(serverSocket, readSet);

        isServer = true;
        isConnected = true;
    }

    void WinNetwork::Disconnect()
    {
        if(IsValidServer())
        {
            std::for_each(clientSockets.begin(), clientSockets.end(), [](SOCKET sock){ closesocket(sock); });
            closesocket(serverSocket);
        }
        else if(IsValidClient())
        {
            closesocket(clientSockets[0]);
        }
    }

    bool WinNetwork::IsServer() const
    {
        return isServer;
    }

    bool WinNetwork::IsConnecting() const
    {
        return isConnecting;
    }

    bool WinNetwork::IsConnected() const
    {
        return isConnected;
    }

    bool WinNetwork::IsValidServer() const
    {
        return isServer && isConnected;
    }

    bool WinNetwork::IsValidClient() const
    {
        return !isServer && isConnected;
    }

    bool WinNetwork::ReadSocket(SOCKET socket)
    {
        return true;
    }

    void WinNetwork::StepLoop()
    {
        if(IsValidServer())
        {
            if(select(0, readSet, nullptr, nullptr, nullptr) == SOCKET_ERROR)
            {
                // uh oh
            }

            for(auto it = clientSockets.begin(); it != clientSockets.end(); ++it)
            {
                auto socket = *it;
                if(FD_ISSET(socket, readSet))
                {
                    //handle disconnect
                    if(ReadSocket(socket))
                    {
                        systems.GetUserinterface().GetConsole().Print("Player disconnected");
                        closesocket(socket);
                        clientSockets.erase(it);
                        FD_CLR(socket, readSet);
                    }
                }
            }

            if(FD_ISSET(serverSocket, readSet))
            {
                struct sockaddr_storage clientSocketAddr;
                socklen_t clientSockSize;
                SOCKET clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientSocketAddr), &clientSockSize);
                if(clientSocket == INVALID_SOCKET)
                {
                    // uh oh
                }

                clientSockets.push_back(clientSocket);
            }
        }
        else if(IsValidClient())
        {
            auto clientSocket = clientSockets[0];
            if(ReadSocket(clientSocket))
            {
                Disconnect();
                //switch to mainmenu
            }
        }
    }
}

#endif
