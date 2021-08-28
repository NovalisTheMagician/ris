#pragma once

#include "game/MapProps.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <memory>

namespace RIS::Game
{
    class MapEntity
    {
    public:
        MapEntity(const std::string &classname, MapProps &&props);

        std::string_view Classname() const;
        const MapProps& Props() const;

    private:
        std::string classname;
        MapProps props;

    };

    using MapEntities = std::vector<MapEntity>;
    using MapEntitiesPtr = std::shared_ptr<std::vector<MapEntity>>;
}
