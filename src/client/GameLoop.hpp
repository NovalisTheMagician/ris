#pragma once

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"
#include "common/Args.hpp"
#include "common/Timer.hpp"

#include <thread>
#include <atomic>

namespace RIS
{
    class GameLoop
    {
    public:
        GameLoop(const SystemLocator &systems, const Config &config, const Args &args);
        ~GameLoop();

        GameLoop(const GameLoop &) = delete;
        GameLoop &operator=(const GameLoop &) = delete;
        GameLoop(GameLoop &&) = delete;
        GameLoop &operator=(GameLoop &&) = delete;

        int Start();

    private:
        void StartThreads();

        void NetworkThread();
        void GameplayThread();

    private:
        const SystemLocator &systems;
        const Args &args;
        const Config &config;

        std::thread networkThread;
        std::atomic_bool runNetworkThread;

        std::thread gameplayThread;
        std::atomic_bool runGameplayThread;

    };
}
