#pragma once

#include "loader/ResourcePack.hpp"

namespace RIS::Game
{
    class GameLoop
    {
    public:
        GameLoop(Loader::ResourcePack &&resourcePack);
        ~GameLoop() = default;
        GameLoop(const GameLoop &) = delete;
        GameLoop &operator=(const GameLoop &) = delete;
        GameLoop(GameLoop &&) = default;
        GameLoop &operator=(GameLoop &&) = default;

        int Start();

    private:
        void InitMenus();

    private:
        Loader::ResourcePack resourcePack;

    };
}
