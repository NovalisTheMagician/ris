#pragma once

#include <functional>
#include <string>

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

    using KeyCallback = std::function<void(int)>;
    using ButtonCallback = KeyCallback;
    using MouseCallback = std::function<void(float, float)>;
    using WheelCallback = std::function<void(float, float)>;
    using CharCallback = std::function<void(char)>;

    class IInput
    {
    public:
        virtual ~IInput() = default;

        virtual void RegisterKeyUp(const std::string &handle, KeyCallback callback) = 0;
        virtual void UnregisterKeyUp(const std::string &handle) = 0;
        virtual void RegisterKeyDown(const std::string &handle, KeyCallback callback) = 0;
        virtual void UnregisterKeyDown(const std::string &handle) = 0;
        virtual void RegisterButtonUp(const std::string &handle, ButtonCallback callback) = 0;
        virtual void UnregisterButtonUp(const std::string &handle) = 0;
        virtual void RegisterButtonDown(const std::string &handle, ButtonCallback callback) = 0;
        virtual void UnregisterButtonDown(const std::string &handle) = 0;
        virtual void RegisterMouse(const std::string &handle, MouseCallback callback) = 0;
        virtual void UnregisterMouse(const std::string &handle) = 0;
        virtual void RegisterWheel(const std::string &handle, WheelCallback callback) = 0;
        virtual void UnregisterWheel(const std::string &handle) = 0;
        virtual void RegisterChar(const std::string &handle, CharCallback callback) = 0;
        virtual void UnregisterChar(const std::string &handle) = 0;

        virtual void Update() = 0;
    };
}
