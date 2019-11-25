#pragma once

namespace RIS
{
    enum class InputAction
    {
        //Movement
        MOVE_FORWARD,
        MOVE_BACKWARD,
        MOVE_LEFT,
        MOVE_RIGHT,
        TURN_HORIZONTAL,
        TURN_VERTICAL,
        JUMP,
        CROUCH,
        RUN,
        //Itemmanagement
        SWITCH_TO_1,
        SWITCH_TO_2,
        SWITCH_TO_3,
        NEXT_ITEM,
        PREV_ITEM,
        //Actions
        DO_PRIMARY,
        DO_SECONDARY,
        DO_TERTIARY,
        USE,
        //Miscellaneous
        OPEN_CONSOLE,
        OPEN_MENU,
        OPEN_INVENTORY,
        OPEN_SKILLS,
        OPEN_NETWORK,
        OPEN_CHAT,
        //Highest index of this enum
        NUM_ACTIONS
    };

    class IInput
    {
    public:
        virtual ~IInput() = default;

        virtual void Update() = 0;
    };
}
