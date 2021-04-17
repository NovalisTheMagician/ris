#include "loader/MapLoader.hpp"

#include "loader/TextureLoader.hpp"

#include "graphics/VertexTypes.hpp"
#include "graphics/MapMesh.hpp"
#include "graphics/Buffer.hpp"

#include <fmt/format.h>

namespace RIS::Loader
{
    struct polyheader
    {
        char magic[4];
        int version;
        int numSections;
    };

    struct polysection
    {
        char texture[64];
        int numPolygons;
    };

    struct polydata
    {
        int numVertices;
        int numIndices;
    };

    template<>
    std::shared_ptr<Graphics::MapMesh> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
    {
        if(bytes.size() == 0)
            return nullptr;

        auto readBytes = [bytesPtr = bytes.data()](auto *dst, std::size_t size) mutable
        {
            std::memcpy(dst, bytesPtr, size);
            bytesPtr += size;
        };

        polyheader header = {};
        readBytes(&header, sizeof header);
        if(header.magic[0] != 'P' || header.magic[1] != 'O' || header.magic[2] != 'L' || header.magic[3] != 'Y')
            return nullptr;
        if(header.version != 1)
            return nullptr;

        std::vector<VertexType::MapVertex> vertices;
        std::vector<std::uint16_t> indices;
        std::vector<Graphics::MapSection> sections;
        for(std::size_t s = 0; s < header.numSections; ++s)
        {
            polysection section = {};
            readBytes(&section, sizeof section);

            std::string textureName(section.texture);
            textureName = fmt::format("textures/{}.dds", textureName);
            auto &sec = sections.emplace_back();
            sec.texture = Load<Graphics::Texture>(resourcePack.Read(textureName), textureName, true, resourcePack);
            sec.offset = indices.size();

            int sectionIndices = 0;
            for(std::size_t p = 0; p < section.numPolygons; ++p)
            {
                polydata data = {};
                readBytes(&data, sizeof data);

                std::vector<VertexType::MapVertex> tmpVertices(data.numVertices);
                readBytes(tmpVertices.data(), data.numVertices * sizeof(VertexType::MapVertex));

                std::uint16_t indexOffset = static_cast<std::uint16_t>(vertices.size());
                vertices.insert(std::cend(vertices), std::begin(tmpVertices), std::end(tmpVertices));

                std::vector<std::uint16_t> tmpIndices(data.numIndices);
                readBytes(tmpIndices.data(), data.numIndices * sizeof(std::uint16_t));

                std::for_each(std::begin(tmpIndices), std::end(tmpIndices), [indexOffset](auto &index){ index += indexOffset; });
                indices.insert(std::cend(indices), std::begin(tmpIndices), std::end(tmpIndices));

                sectionIndices += data.numIndices;
            }
            sec.count = static_cast<std::size_t>(sectionIndices);
        }

        Graphics::VertexBuffer vertexBuffer(vertices);
        Graphics::IndexBuffer indexBuffer(indices);
        Graphics::MapMesh::Ptr mapMesh = std::make_shared<Graphics::MapMesh>(std::move(vertexBuffer), std::move(indexBuffer), std::move(sections));
        return mapMesh;
    }
}
