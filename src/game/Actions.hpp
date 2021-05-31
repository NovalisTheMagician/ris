#pragma once

namespace RIS::Game
{
    enum class Action
    {
        MOVE_FORWARD,
        MOVE_BACKWARD,
        STRAFE_LEFT,
        STRAFE_RIGHT,
        JUMP,
        CROUCH,
        
        OPEN_MENU,

        QUICK_SAVE,
        QUICK_LOAD,
        QUICK_EXIT,
    };
}
