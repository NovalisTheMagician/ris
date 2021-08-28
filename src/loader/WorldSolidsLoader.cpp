#include "loader/WorldSolidsLoader.hpp"

#include "physics/WorldSolids.hpp"

#include <fmt/format.h>
#include <string_view>
#include <cstdint>

namespace RIS::Loader
{
    constexpr char BRUSH_FILE_MAGIC[4] = {'B', 'R', 'U', 'S'};
    constexpr uint32_t BRUSH_FILE_VERSION = 1;

    struct brushheader
    {
        char magic[4];
        uint32_t version;
        uint32_t numBrushes;
    };

    struct brushdata
    {
        uint32_t numPlanes;
    };

    template<>
    std::shared_ptr<Physics::WorldSolids> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
    {
        if(bytes.size() == 0)
            return nullptr;

        auto readBytes = [bytesPtr = bytes.data()](auto *dst, std::size_t size) mutable
        {
            std::memcpy(dst, bytesPtr, size);
            bytesPtr += size;
        };

        brushheader header = {};
        readBytes(&header, sizeof header);
        if(std::string_view(header.magic, sizeof header.magic) != std::string_view(BRUSH_FILE_MAGIC, sizeof BRUSH_FILE_MAGIC))
            return nullptr;
        if(header.version != BRUSH_FILE_VERSION)
            return nullptr;

        std::vector<Physics::Brush> brushes;
        for(uint32_t b = 0; b < header.numBrushes; ++b)
        {
            brushdata data = {};
            readBytes(&data, sizeof data);

            auto &brush = brushes.emplace_back();
            for(uint32_t p = 0; p < data.numPlanes; ++p)
            {
                Physics::Plane plane = {};
                readBytes(&plane, sizeof plane);
                brush.planes.push_back(plane);
            }
        }

        Physics::WorldSolids::Ptr worldSolids = std::make_shared<Physics::WorldSolids>(std::move(brushes));
        return worldSolids;
    }
}
