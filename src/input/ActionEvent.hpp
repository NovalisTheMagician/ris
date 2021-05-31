#pragma once

#include <variant>

#include <glm/glm.hpp>

namespace RIS::Input
{
    enum class EventType
    {
        KEY,
        MOUSE_MOVE,
        MOUSE_WHEEL
    };

    enum class EventState
    {
        UP,
        DOWN
    };

    template<typename Action>
    struct BinaryEvent
    {
        EventState state;
        Action action;
    };

    template<typename Action>
    struct ActionEvent
    {
        EventType type;
        std::variant<BinaryEvent<Action>, glm::vec2> value;
    };
}
