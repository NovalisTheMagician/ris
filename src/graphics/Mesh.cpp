#include "graphics/Mesh.hpp"

#include <glad2/gl.h>

namespace RIS::Graphics
{
    Mesh::Mesh(Buffer &&vertexBuffer, Buffer &&indexBuffer, int numIndices)
        : vertexBuffer(std::move(vertexBuffer))
        , indexBuffer(std::move(indexBuffer))
        , numIndices(numIndices)
    {}

    void Mesh::Bind(VertexArray &vao) const
    {
        vao.SetVertexBuffer<VertexType::ModelVertex>(vertexBuffer);
        vao.SetIndexBuffer(indexBuffer);
    }

    const Buffer& Mesh::GetVertexBuffer() const
    {
        return vertexBuffer;
    }

    const Buffer& Mesh::GetIndexBuffer() const
    {
        return indexBuffer;
    }

    int Mesh::NumIndices() const
    {
        return numIndices;
    }

    void Mesh::Draw(int count, int offset) const
    {
        count = count == -1 ? numIndices : count;
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(offset * sizeof(std::uint16_t)));
    }
}
