#pragma once

#include "graphics/Buffer.hpp"
#include "graphics/VertexTypes.hpp"
#include "graphics/VertexArray.hpp"

namespace RIS
{
    namespace Graphics
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

            void Bind(VertexArray &vao);
            void Draw() const;

            const Buffer& VertexBuffer() const;
            const Buffer& IndexBuffer() const;
            int NumIndices() const;

        private:
            Buffer vertexBuffer;
            Buffer indexBuffer;
            const int numIndices;
        };
    }
}