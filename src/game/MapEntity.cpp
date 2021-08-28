#include "game/MapEntity.hpp"

namespace RIS::Game
{
    MapEntity::MapEntity(const std::string &classname, MapProps &&props)
        : classname(classname), props(std::move(props))
    {}

    std::string_view MapEntity::Classname() const
    {
        return classname;
    }

    const MapProps& MapEntity::Props() const
    {
        return props;
    }
}
