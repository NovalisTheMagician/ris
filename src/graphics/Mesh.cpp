#include "graphics/Mesh.hpp"

#include <glad/glad.h>

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

    void Mesh::Draw() const
    {
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
    }
}
