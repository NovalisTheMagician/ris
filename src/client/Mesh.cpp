#include "Mesh.hpp"

namespace RIS
{
    Mesh::Mesh(const Buffer<VertexType::ModelVertex> &vertexBuffer, const Buffer<uint16_t> &indexBuffer, int numIndices, bool isStatic)
        : vertexBuffer(vertexBuffer), indexBuffer(indexBuffer), numIndices(numIndices), isStatic(isStatic)
    {

    }

    void Mesh::Bind(VertexArray &vao)
    {
        vao.SetVertexBuffer(vertexBuffer, 0);
        vao.SetIndexBuffer(indexBuffer);
    }

    const Buffer<VertexType::ModelVertex>& Mesh::VertexBuffer() const
    {
        return vertexBuffer;
    }

    const Buffer<uint16_t>& Mesh::IndexBuffer() const
    {
        return indexBuffer;
    }

    int Mesh::NumIndices() const
    {
        return numIndices;
    }
}
