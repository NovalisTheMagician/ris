#pragma once

#include <string_view>
#include <algorithm>
#include <optional>
#include <fstream>
#include <array>

#include <glm/glm.hpp>

#include <magic_enum.hpp>

#include "input/KeyDefs.hpp"
#include "input/ActionEvent.hpp"

#include <iostream>

namespace RIS::Input
{
    constexpr std::string_view BINDINGS_FILE_NAME = "keyboard_bindings.cfg";

    struct ActionState
    {
        bool held, pressed, released;
    };

    template<typename Action>
    class InputMapper
    {
    public:
        InputMapper()
        {
            std::fill(std::begin(mapping), std::end(mapping), InputKey::NONE);
            std::fill(std::begin(currentStates), std::end(currentStates), false);
            std::fill(std::begin(previousStates), std::end(previousStates), false);
        }

        InputMapper(std::string_view mappingFile)
        {
            std::fill(std::begin(mapping), std::end(mapping), InputKey::NONE);
            std::fill(std::begin(currentStates), std::end(currentStates), false);
            std::fill(std::begin(previousStates), std::end(previousStates), false);

            std::ifstream mappingStream(mappingFile);
            if(mappingStream)
            {
                // read input mappings
            }
        }

        void Set(Action action, InputKey key)
        {
            auto index = magic_enum::enum_index<Action>(action);
            if(index)
                mapping[*index] = key;
        }

        InputKey Get(Action action) const
        {
            auto index = magic_enum::enum_index<Action>(action);
            if(index)
                return mapping[*index];
            return InputKey::NONE;
        }

        std::optional<Action> Get(InputKey key) const
        {
            auto found = std::find(std::begin(mapping), std::end(mapping), key);
            if(found != std::end(mapping))
            {
                std::size_t index = std::distance(std::begin(mapping), found);
                return magic_enum::enum_value<Action>(index);
            }
            return std::nullopt;
        }

        void Clear()
        {
            std::fill(std::begin(mapping), std::end(mapping), InputKey::NONE);
        }

        bool IsEmpty() const
        {
            return std::all_of(std::begin(mapping), std::end(mapping), [](const auto &v){ return v == InputKey::NONE; });
        }

        void SaveMapping(std::string_view mappingFile) const
        {
            std::ofstream output(mappingFile);
            if(output)
            {
                for(std::size_t i = 0; i < mapping.size(); ++i)
                {
                    auto key = mapping[i];
                    auto action = magic_enum::enum_value<Action>(i);

                    output << magic_enum::enum_name<Action>(action) << " = " << ToString(key) << "\n";
                }
            }
        }

        ActionState GetState(Action action) const
        {
            auto index = magic_enum::enum_index<Action>(action);
            if(index)
                return actionStates[*index];
            return {false, false, false};
        }

        glm::vec2 GetMouse() const
        {
            return positionDiff;
        }

        glm::vec2 GetWheel() const
        {
            return wheelDiff;
        }

        void Update()
        {
            for (std::size_t i = 0; i < actionStates.size(); ++i)
            {
                ActionState& state = actionStates.at(i);
                state.held = currentStates[i];
                state.pressed = currentStates[i] && !previousStates[i];
                state.released = !currentStates[i] && previousStates[i];
                previousStates[i] = currentStates[i];
            }

            positionDiff = currentPosition - lastPosition;
            lastPosition = currentPosition;
            wheelDiff = currentWheel - lastWheel;
            lastWheel = currentWheel;
        }

        void OnInputDown(InputKey key)
        {
            auto action = Get(key);
            if(action)
            {
                auto index = magic_enum::enum_index<Action>(*action);
                if(index)
                    currentStates[*index] = true;
            }
        }

        void OnInputUp(InputKey key)
        {
            auto action = Get(key);
            if(action)
            {
                auto index = magic_enum::enum_index<Action>(*action);
                if(index)
                    currentStates[*index] = false;
            }
        }

        void OnMouseMove(float x, float y)
        {
            currentPosition = glm::vec2(x, y);
        }

        void OnMouseWheel(float x, float y)
        {
            currentWheel = glm::vec2(x, y);
        }

    private:
        std::array<InputKey, magic_enum::enum_count<Action>()> mapping;

        std::array<bool, magic_enum::enum_count<Action>()> currentStates;
        std::array<bool, magic_enum::enum_count<Action>()> previousStates;
        std::array<ActionState, magic_enum::enum_count<Action>()> actionStates;

        glm::vec2 currentPosition;
        glm::vec2 lastPosition;
        glm::vec2 positionDiff;

        glm::vec2 currentWheel;
        glm::vec2 lastWheel;
        glm::vec2 wheelDiff;

    };
}
