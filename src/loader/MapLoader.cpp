#include "loader/MapLoader.hpp"

#include "loader/TextureLoader.hpp"

#include "graphics/VertexTypes.hpp"
#include "graphics/MapMesh.hpp"
#include "graphics/Buffer.hpp"

#include <fmt/format.h>
#include <string_view>
#include <cstdint>

namespace RIS::Loader
{
    constexpr char POLY_FILE_MAGIC[4] = {'P', 'O', 'L', 'Y'};
    constexpr uint32_t POLY_FILE_VERSION = 1;

    struct polyheader
    {
        char magic[4];
        uint32_t version;
        uint32_t numSections;
    };

    struct polysection
    {
        char texture[64];
        uint32_t numPolygons;
    };

    struct polydata
    {
        uint32_t numVertices;
        uint32_t numIndices;
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
        if(std::string_view(header.magic, sizeof header.magic) != std::string_view(POLY_FILE_MAGIC, sizeof POLY_FILE_MAGIC))
            return nullptr;
        if(header.version != POLY_FILE_VERSION)
            return nullptr;

        std::vector<VertexType::MapVertex> vertices;
        std::vector<std::uint16_t> indices;
        std::vector<Graphics::MapSection> sections;
        for(uint32_t s = 0; s < header.numSections; ++s)
        {
            polysection section = {};
            readBytes(&section, sizeof section);

            std::string textureName(section.texture);
            textureName = fmt::format("textures/{}.dds", textureName);
            auto &sec = sections.emplace_back();
            sec.texture = Load<Graphics::Texture>(resourcePack.Read(textureName), textureName, false, resourcePack);
            sec.offset = indices.size();

            int sectionIndices = 0;
            for(uint32_t p = 0; p < section.numPolygons; ++p)
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
