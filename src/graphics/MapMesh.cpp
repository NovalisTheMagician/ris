#include "graphics/MapMesh.hpp"

namespace RIS::Graphics
{
    MapMesh::MapMesh(Buffer &&vertexBuffer, Buffer &&indexBuffer, std::vector<MapSection> &&sections)
        : vertexBuffer(std::move(vertexBuffer))
        , indexBuffer(std::move(indexBuffer))
        , sections(std::move(sections))
    {}

    void MapMesh::Bind(VertexArray &vao) const
    {
        vao.SetVertexBuffer<VertexType::MapVertex>(vertexBuffer);
        vao.SetIndexBuffer(indexBuffer);
    }

    void MapMesh::Draw() const
    {
        for(const auto &section : sections)
        {
            section.texture->Bind(0);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(section.count), GL_UNSIGNED_SHORT, reinterpret_cast<void*>(section.offset * sizeof(std::uint16_t)));
        }
    }
}
