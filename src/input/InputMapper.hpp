#pragma once

#include <unordered_map>
#include <string_view>
#include <functional>
#include <variant>
#include <fstream>

#include <glm/glm.hpp>

#include <magic_enum.hpp>

#include "input/KeyDefs.hpp"
#include "input/ActionEvent.hpp"

namespace RIS::Input
{
    constexpr std::string_view BINDINGS_FILE_NAME = "keyboard_bindings.cfg";

    template<typename Action>
    class InputMapper
    {
    public:
        using ActionFunc = std::function<void(ActionEvent<Action>)>;

    public:
        InputMapper() : callback([](auto){}) {}
        InputMapper(std::string_view mappingFile)
            : callback([](auto){})
        {
            std::ifstream mappingStream(mappingFile);
            if(mappingStream)
            {
                // read input mappings
            }
        }

        template<typename T>
        void SetCallback(T func)
        {
            callback = func;
        }

        void Map(InputKey key, Action action)
        {
            auto found = std::find_if(std::begin(primaryMapping), std::end(primaryMapping), [action](const auto &v){ return v.second == action; });
            if(found != std::end(primaryMapping))
                primaryMapping.erase(found);
            primaryMapping.insert_or_assign(key, action);
        }

        void Clear(Action action)
        {
            auto found = std::find_if(std::begin(primaryMapping), std::end(primaryMapping), [action](const auto &v){ return v.second == action; });
            if(found != std::end(primaryMapping))
                primaryMapping.erase(found);
        }

        void Clear()
        {
            primaryMapping.clear();
        }

        bool IsEmpty() const
        {
            return primaryMapping.empty();
        }

        void SaveMapping(std::string_view mappingFile) const
        {
            std::ofstream output(mappingFile);
            if(output)
            {
                for(auto it = std::begin(primaryMapping); it != std::end(primaryMapping); ++it)
                {
                    auto key = (*it).first;
                    auto action = (*it).second;

                    output << magic_enum::enum_name<Action>(action) << " = " << ToString(key) << "\n";
                }
            }
        }

        void OnInputDown(InputKey key) const
        {
            auto found = primaryMapping.find(key);
            if(found != std::end(primaryMapping))
            {
                ActionEvent<Action> e = { EventType::KEY, BinaryEvent<Action>{ EventState::DOWN, (*found).second } };
                callback(e);
            }
        }

        void OnInputUp(InputKey key) const
        {
            auto found = primaryMapping.find(key);
            if(found != std::end(primaryMapping))
            {
                ActionEvent<Action> e = { EventType::KEY, BinaryEvent<Action>{ EventState::UP, (*found).second } };
                callback(e);
            }
        }

        void OnMouseMove(float x, float y) const
        {
            ActionEvent<Action> e = { EventType::MOUSE_MOVE, glm::vec2{ x, y } };
            callback(e);
        }

        void OnMouseWheel(float x, float y) const
        {
            ActionEvent<Action> e = { EventType::MOUSE_WHEEL, glm::vec2{ x, y } };
            callback(e);
        }

    private:
        std::unordered_map<InputKey, Action> primaryMapping;
        //std::unordered_map<InputKey, Action> secondaryMapping;

        ActionFunc callback;

    };
}
