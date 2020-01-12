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

    enum class InputButtons
    {
        NONE,
        LEFT,
        RIGHT,
        MIDDLE,
        EXTRA1,
        EXTRA2,
        EXTRA3,
        EXTRA4,
        EXTRA5,
    };

    enum class InputKeys
    {
        NONE,
        SPACE,
        APOSTROPHE,
        COMMA,
        MINUS,
        PERIOD,
        SLASH,
        NUM_0,
        NUM_1,
        NUM_2,
        NUM_3,
        NUM_4,
        NUM_5,
        NUM_6,
        NUM_7,
        NUM_8,
        NUM_9,
        SEMICOLON,
        EQUALS,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        LEFT_BRACKET,
        BACKSLASH,
        RIGHT_BRACKET,
        GRAVE_ACCENT,
        WORLD_1,
        WORLD_2,
        ESCAPE,
        ENTER,
        TAB,
        BACKSPACE,
        INSERT,
        DELETE,
        RIGHT,
        LEFT,
        DOWN,
        UP,
        PAGE_UP,
        PAGE_DOWN,
        HOME,
        END,
        CAPS_LOCK,
        SCROLL_LOCK,
        NUM_LOCK,
        PRINT_SCREEN,
        PAUSE,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,
        KP_0,
        KP_1,
        KP_2,
        KP_3,
        KP_4,
        KP_5,
        KP_6,
        KP_7,
        KP_8,
        KP_9,
        KP_DECIMAL,
        KP_DIVIDE,
        KP_MULTIPLY,
        KP_SUBTRACT,
        KP_ADD,
        KP_ENTER,
        KP_EQUAL,
        LEFT_SHIFT,
        LEFT_CONTROL,
        LEFT_ALT,
        LEFT_SUPER,
        RIGHT_SHIFT,
        RIGHT_CONTROL,
        RIGHT_ALT,
        RIGHT_SUPER,
        MENU
    };

    using KeyCallback = std::function<void(InputKeys)>;
    using ButtonCallback = std::function<void(InputButtons)>;
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
