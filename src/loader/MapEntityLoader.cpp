#include "loader/MapEntityLoader.hpp"

#include "game/MapEntity.hpp"
#include "game/MapProps.hpp"

#include "physics/Brush.hpp"

#include "graphics/VertexTypes.hpp"

#include <string>
#include <string_view>
#include <cstdint>

#include <glm/glm.hpp>

namespace RIS::Loader
{
    constexpr char ENTITY_FILE_MAGIC[4] = {'E', 'N', 'T', 'I'};
    constexpr uint32_t ENTITY_FILE_VERSION = 1;

    struct entityheader
    {
        char magic[4];
        uint32_t version;
        uint32_t numEntities;
    };

    struct entityentry
    {
        char entityName[64];
        uint32_t numProperties;
    };

    struct entityproperty
    {
        char propertyName[64];
        uint32_t valueType;
        //value
    };

    struct entitypolygonentry
    {
        uint32_t numPolygonSections;
    };

    struct entitypolygonsection
    {
        char texture[64];
        uint32_t numPolygons;
    };

    struct entitypolygon
    {
        uint32_t numVertices;
        uint32_t numIndices;
        //vertices
    };

    struct entitybrushentry
    {
        uint32_t numBrushes;
    };

    struct entitybrush
    {
        uint32_t numPlanes;
        //planes
    };

    enum class value_type : uint32_t
    {
        vt_string,
        vt_int,
        vt_float,
        vt_vec2,
        vt_vec3,
        vt_bool
    };

    template<>
    std::shared_ptr<std::vector<Game::MapEntity>> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
    {
        if(bytes.size() == 0)
            return nullptr;

        auto readBytes = [bytesPtr = bytes.data()](auto *dst, std::size_t size) mutable
        {
            std::memcpy(dst, bytesPtr, size);
            bytesPtr += size;
        };

        entityheader header = {};
        readBytes(&header, sizeof header);
        if(std::string_view(header.magic, sizeof header.magic) != std::string_view(ENTITY_FILE_MAGIC, sizeof ENTITY_FILE_MAGIC))
            return nullptr;
        if(header.version != ENTITY_FILE_VERSION)
            return nullptr;

        std::shared_ptr<std::vector<Game::MapEntity>> entList = std::make_unique<std::vector<Game::MapEntity>>();
        auto &entities = *entList;
        for(uint32_t i = 0; i < header.numEntities; ++i)
        {
            entityentry entEntry = {};
            readBytes(&entEntry, sizeof entEntry);
            std::string classname(entEntry.entityName/*, sizeof entEntry.entityName*/);

            Game::MapProps props;
            for(uint32_t j = 0; j < entEntry.numProperties; ++j)
            {
                entityproperty entProp = {};
                readBytes(&entProp, sizeof entProp);
                std::string key(entProp.propertyName/*, sizeof entProp.propertyName*/);
                switch (static_cast<value_type>(entProp.valueType))
                {
                case value_type::vt_bool:
                    {
                        bool val;
                        readBytes(&val, sizeof val);
                        props.Set(key, val);
                    }
                    break;
                case value_type::vt_int:
                    {
                        int val;
                        readBytes(&val, sizeof val);
                        props.Set(key, val);
                    }
                    break;
                case value_type::vt_float:
                    {
                        float val;
                        readBytes(&val, sizeof val);
                        props.Set(key, val);
                    }
                    break;
                case value_type::vt_vec2:
                    {
                        glm::vec2 val;
                        readBytes(&val, sizeof val);
                        props.Set(key, val);
                    }
                    break;
                case value_type::vt_vec3:
                    {
                        glm::vec3 val;
                        readBytes(&val, sizeof val);
                        props.Set(key, val);
                    }
                    break;
                case value_type::vt_string:
                    {
                        constexpr int MAX_CHARS = 128;
                        char buffer[MAX_CHARS];
                        readBytes(buffer, MAX_CHARS);
                        std::string val(buffer/*, MAX_CHARS*/);
                        props.Set(key, val);
                    }
                    break;
                }
            }

            auto &entity = entities.emplace_back(classname, std::move(props));

            entitypolygonentry polyEntry = {};
            readBytes(&polyEntry, sizeof polyEntry);

            for(uint32_t j = 0; j < polyEntry.numPolygonSections; ++j)
            {
                entitypolygonsection polySection = {};
                readBytes(&polySection, sizeof polySection);
                std::string textureName(polySection.texture, sizeof polySection.texture);

                for(uint32_t k = 0; k < polySection.numPolygons; ++k)
                {
                    entitypolygon poly = {};
                    readBytes(&poly, sizeof poly);

                    std::vector<VertexType::MapVertex> vertices(poly.numVertices);
                    std::vector<uint16_t> indices(poly.numIndices);

                    readBytes(vertices.data(), poly.numVertices * sizeof(VertexType::MapVertex));
                    readBytes(indices.data(), poly.numIndices * sizeof(uint16_t));
                }
            }

            entitybrushentry brushentry = {};
            readBytes(&brushentry, sizeof brushentry);

            for(uint32_t j = 0; j < brushentry.numBrushes; ++j)
            {
                entitybrush brush = {};
                readBytes(&brush, sizeof brush);

                for(uint32_t k = 0; k < brush.numPlanes; ++k)
                {
                    Physics::Plane plane;
                    readBytes(&plane, sizeof plane);
                }
            }
        }

        return entList;
    }
}
