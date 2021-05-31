#include "game/MapProps.hpp"

#include "RisExcept.hpp"

namespace RIS::Game
{
    MapProps::MapProps(const std::vector<std::byte> &bytes)
    {
        
    }

    void MapProps::Set(const std::string &key, PropType value)
    {
        props.insert_or_assign(key, value);
    }
}
