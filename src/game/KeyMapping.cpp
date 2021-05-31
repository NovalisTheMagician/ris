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
            inputMapper.Map(Input::InputKey::W, Action::MOVE_FORWARD);
            inputMapper.Map(Input::InputKey::S, Action::MOVE_BACKWARD);
            inputMapper.Map(Input::InputKey::A, Action::STRAFE_LEFT);
            inputMapper.Map(Input::InputKey::D, Action::STRAFE_RIGHT);
            inputMapper.Map(Input::InputKey::SPACE, Action::JUMP);
            inputMapper.Map(Input::InputKey::LEFT_CONTROL, Action::CROUCH);

            inputMapper.Map(Input::InputKey::ESCAPE, Action::OPEN_MENU);

            inputMapper.Map(Input::InputKey::F5, Action::QUICK_SAVE);
            inputMapper.Map(Input::InputKey::F9, Action::QUICK_LOAD);
            inputMapper.Map(Input::InputKey::F7, Action::QUICK_EXIT);

            auto path = Window::GetConfigPath();
            path /= Input::BINDINGS_FILE_NAME;
            inputMapper.SaveMapping(path.generic_string());
        }
    }
}
