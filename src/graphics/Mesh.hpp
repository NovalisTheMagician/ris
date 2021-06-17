#pragma once

#include "graphics/Buffer.hpp"
#include "graphics/VertexTypes.hpp"
#include "graphics/VertexArray.hpp"

#include <memory>

namespace RIS::Graphics
{
    class Mesh
    {
    public:
        using Ptr = std::shared_ptr<Mesh>;

        Mesh(Buffer &&vertexBuffer, Buffer &&indexBuffer, int numIndices);

        Mesh() = default;
        Mesh(const Mesh &) = delete;
        Mesh(Mesh &&) = default;
        Mesh &operator=(const Mesh &) = delete;
        Mesh &operator=(Mesh &&) = default;

        void Bind(VertexArray &vao) const;
        void Draw(int count = -1, int offset = 0) const;

        const Buffer& GetVertexBuffer() const;
        const Buffer& GetIndexBuffer() const;
        int NumIndices() const;

    private:
        Buffer vertexBuffer;
        Buffer indexBuffer;
        int numIndices;
    };
}
