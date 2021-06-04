#pragma once

#include <string_view>

#include "loader/ResourcePack.hpp"

#include "input/KeyDefs.hpp"
#include "input/InputMapper.hpp"

#include "game/State.hpp"
#include "game/Actions.hpp"

namespace RIS::Game
{
    class GameLoop
    {
    public:
        GameLoop(Loader::ResourcePack &&resourcePack, std::string_view loadMap = "maps/menu");
        ~GameLoop() = default;
        GameLoop(const GameLoop &) = delete;
        GameLoop &operator=(const GameLoop &) = delete;
        GameLoop(GameLoop &&) = default;
        GameLoop &operator=(GameLoop &&) = default;

        int Start();

    private:
        void RegisterFuncs();
        void InitMenus();
        void InitKeyMapping();

    private:
        Loader::ResourcePack resourcePack;
        Input::InputMapper<Action> inputMapper;
        State state;

    };
}
