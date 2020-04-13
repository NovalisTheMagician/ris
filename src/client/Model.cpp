#include "Model.hpp"

namespace RIS
{
    Model::Model(const Buffer<VertexType::ModelVertex> &vertexBuffer, const Buffer<uint16_t> &indexBuffer, int numIndices, bool isStatic)
        : vertexBuffer(vertexBuffer), indexBuffer(indexBuffer), numIndices(numIndices), isStatic(isStatic)
    {

    }

    void Model::Bind(VertexArray &vao)
    {
        vao.SetVertexBuffer(vertexBuffer, 0);
        vao.SetIndexBuffer(indexBuffer);
    }

    const Buffer<VertexType::ModelVertex>& Model::VertexBuffer() const
    {
        return vertexBuffer;
    }

    const Buffer<uint16_t>& Model::IndexBuffer() const
    {
        return indexBuffer;
    }

    int Model::NumIndices() const
    {
        return numIndices;
    }
}
