#include "Mesh.hpp"

namespace RIS
{
    Mesh::Mesh(const Buffer &vertexBuffer, const Buffer &indexBuffer, int numIndices, bool isStatic)
        : vertexBuffer(vertexBuffer), indexBuffer(indexBuffer), numIndices(numIndices), isStatic(isStatic)
    {

    }

    void Mesh::Bind(VertexArray &vao)
    {
        vao.SetVertexBuffer<VertexType::ModelVertex>(vertexBuffer);
        vao.SetIndexBuffer(indexBuffer);
    }

    const Buffer& Mesh::VertexBuffer() const
    {
        return vertexBuffer;
    }

    const Buffer& Mesh::IndexBuffer() const
    {
        return indexBuffer;
    }

    int Mesh::NumIndices() const
    {
        return numIndices;
    }
}
