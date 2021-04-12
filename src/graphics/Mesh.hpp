#pragma once

#include "graphics/Buffer.hpp"
#include "graphics/VertexTypes.hpp"
#include "graphics/VertexArray.hpp"

namespace RIS::Graphics
{
    class Mesh
    {
    public:
        Mesh(Buffer &&vertexBuffer, Buffer &&indexBuffer, int numIndices);

        Mesh() = default;
        Mesh(const Mesh &) = delete;
        Mesh(Mesh &&) = default;
        Mesh &operator=(const Mesh &) = delete;
        Mesh &operator=(Mesh &&) = default;

        void Bind(VertexArray &vao) const;
        void Draw() const;

        const Buffer& GetVertexBuffer() const;
        const Buffer& GetIndexBuffer() const;
        int NumIndices() const;

    private:
        Buffer vertexBuffer;
        Buffer indexBuffer;
        const int numIndices;
    };
}
