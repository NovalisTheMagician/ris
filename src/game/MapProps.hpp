#pragma once

#include <unordered_map>
#include <variant>
#include <string>
#include <string_view>
#include <optional>
#include <vector>

#include <glm/glm.hpp>


namespace RIS::Game
{
    enum class MapMode
    {
        MAINMENU,
        SINGLEPLAYER,
    };

    class MapProps
    {
    private:
        using PropType = std::variant<std::string, int, float, glm::vec2, glm::vec3, bool>;;

    public:
        void Set(const std::string &key, PropType value);

        template<typename T>
        std::optional<T> Get(const std::string &key) const
        {
            auto found = props.find(key);
            if(found != std::end(props))
            {
                auto variant = (*found).second;
                if(auto value = std::get_if<T>(&variant))
                {
                    return *value;
                }
            }
            return std::nullopt;
        }

        template<typename T>
        T GetOrDefault(const std::string &key, T def) const
        {
            if(auto value = Get<T>(key)) return *value;
            return def;
        }

    private:
        std::unordered_map<std::string, PropType> props;

    };
}
