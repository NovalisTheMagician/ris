#include "loader/WorldSolidsLoader.hpp"

#include "physics/WorldSolids.hpp"

#include <fmt/format.h>

namespace RIS::Loader
{
    struct brushheader
    {
        char magic[4];
        int version;
        int numBrushes;
    };

    struct brushdata
    {
        int numPlanes;
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
        if(header.magic[0] != 'B' || header.magic[1] != 'R' || header.magic[2] != 'U' || header.magic[3] != 'S')
            return nullptr;
        if(header.version != 1)
            return nullptr;

        std::vector<Physics::Brush> brushes;
        for(int b = 0; b < header.numBrushes; ++b)
        {
            brushdata data = {};
            readBytes(&data, sizeof data);

            auto &brush = brushes.emplace_back();
            for(int p = 0; p < data.numPlanes; ++p)
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
