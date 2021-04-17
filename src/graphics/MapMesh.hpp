#pragma once

#include "graphics/Buffer.hpp"
#include "graphics/VertexTypes.hpp"
#include "graphics/VertexArray.hpp"
#include "graphics/Texture.hpp"

#include <vector>

namespace RIS::Graphics
{
    struct MapSection
    {
        Texture::Ptr texture;
        size_t count;
        size_t offset;
    };

    class MapMesh
    {
    public:
        using Ptr = std::shared_ptr<MapMesh>;

        MapMesh(Buffer &&vertexBuffer, Buffer &&indexBuffer, std::vector<MapSection> &&sections);

        MapMesh(const MapMesh &) = delete;
        MapMesh(MapMesh &&) = default;
        MapMesh &operator=(const MapMesh &) = delete;
        MapMesh &operator=(MapMesh &&) = default;

        void Bind(VertexArray &vao) const;
        void Draw() const;

    private:
        Buffer vertexBuffer;
        Buffer indexBuffer;
        std::vector<MapSection> sections;

    };
}
