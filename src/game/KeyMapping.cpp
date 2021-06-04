#include "game/GameLoop.hpp"

#include "game/Actions.hpp"
#include "input/KeyDefs.hpp"

#include "window/Paths.hpp"

namespace RIS::Game
{
    void GameLoop::InitKeyMapping()
    {
        if(inputMapper.IsEmpty())
        {
            inputMapper.Set(Action::MOVE_FORWARD, Input::InputKey::W);
            inputMapper.Set(Action::MOVE_BACKWARD, Input::InputKey::S);
            inputMapper.Set(Action::STRAFE_LEFT, Input::InputKey::A);
            inputMapper.Set(Action::STRAFE_RIGHT, Input::InputKey::D);
            inputMapper.Set(Action::JUMP, Input::InputKey::SPACE);
            inputMapper.Set(Action::CROUCH, Input::InputKey::LEFT_CONTROL);

            inputMapper.Set(Action::OPEN_MENU, Input::InputKey::ESCAPE);

            inputMapper.Set(Action::QUICK_SAVE, Input::InputKey::F5);
            inputMapper.Set(Action::QUICK_LOAD, Input::InputKey::F9);
            inputMapper.Set(Action::QUICK_EXIT, Input::InputKey::F7);

            auto path = Window::GetConfigPath();
            path /= Input::BINDINGS_FILE_NAME;
            inputMapper.SaveMapping(path.generic_string());
        }
    }
}
