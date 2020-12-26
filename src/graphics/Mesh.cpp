#include "graphics/Mesh.hpp"

#include <glad/glad.h>

namespace RIS::Graphics
{
    Mesh::Mesh(Buffer &&positionBuffer, Buffer &&normalBuffer, Buffer &&texCoordBuffer, Buffer &&jointsBuffer, Buffer &&weightsBuffer, Buffer &&indexBuffer, int numIndices)
        : positionBuffer(std::move(positionBuffer))
        , normalBuffer(std::move(normalBuffer))
        , texCoordBuffer(std::move(texCoordBuffer))
        , jointsBuffer(std::move(jointsBuffer))
        , weightsBuffer(std::move(weightsBuffer))
        , indexBuffer(std::move(indexBuffer))
        , numIndices(numIndices)
    {}

    void Mesh::Bind(VertexArray &vao) const
    {
        vao.SetVertexBuffer<glm::vec3>(positionBuffer, 0);
        vao.SetVertexBuffer<glm::vec3>(normalBuffer, 1);
        vao.SetVertexBuffer<glm::vec2>(texCoordBuffer, 2);
        vao.SetVertexBuffer<glm::i16vec4>(jointsBuffer, 3);
        vao.SetVertexBuffer<glm::vec4>(weightsBuffer, 4);
        vao.SetIndexBuffer(indexBuffer);
    }

    const Buffer& Mesh::GetVertexBuffer() const
    {
        return positionBuffer;
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
