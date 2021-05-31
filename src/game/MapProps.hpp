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
        using PropType = std::variant<int, float, std::string, glm::vec2, glm::vec3>;

    public:
        MapProps() = default;
        MapProps(const std::vector<std::byte> &bytes);

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

    private:
        std::unordered_map<std::string, PropType> props;

    };
}
