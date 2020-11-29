#pragma once

#include "graphics/GLObject.hpp"

#include <stdexcept>

#include <glad/glad.h>

#include <vector>

namespace RIS
{
    namespace Graphics
    {
        class Buffer : public GLObject
        {
        public:
            Buffer(const void *data, size_t size, GLenum usage);
            Buffer(size_t size, GLenum usage);
            template<typename T, std::size_t Size = sizeof T>
            Buffer(const T &data, GLenum usage) : Buffer(&data, Size, usage) {}
            template<typename T, std::size_t Size = sizeof T>
            Buffer(const std::vector<T> &data, GLenum usage) : Buffer(data.data(), data.size() * Size, usage) {}

            Buffer();
            ~Buffer();

            Buffer(const Buffer &) = delete;
            Buffer& operator=(const Buffer &) = delete;

            Buffer(Buffer &&other);
            Buffer& operator=(Buffer &&other);

            void UpdateData(const void *data, size_t size, size_t offset = 0);

            template<typename T, std::size_t Size = sizeof T>
            void UpdateData(const T &data, size_t offset = 0) { UpdateData(&data, Size, offset); }
            template<typename T, std::size_t Size = sizeof T>
            void UpdateData(const std::vector<T> &data, size_t offset = 0) { UpdateData(data.data(), data.size() * Size, offset); }

            void Bind(GLenum target, int bindBase) const;

            void* Map(GLenum access, size_t size = 0, size_t offset = 0);
            void UnMap();
            void Flush(size_t size = 0, size_t offset = 0);

            size_t GetSize() const;

        private:
            size_t maxSize;

        };

        class UniformBuffer : public Buffer
        {
        public:
            UniformBuffer() : Buffer() {}
            UniformBuffer(const void *data, size_t size) : Buffer(data, size, GL_DYNAMIC_STORAGE_BIT) {}
            UniformBuffer(size_t size) : Buffer(size, GL_DYNAMIC_STORAGE_BIT) {}
            template<typename T, std::size_t Size = sizeof T>
            UniformBuffer(const T &data) : Buffer(&data, Size, GL_DYNAMIC_STORAGE_BIT) {}
            template<typename T, std::size_t Size = sizeof T>
            UniformBuffer(const std::vector<T> &data) : Buffer(data.data(), data.size() * Size, GL_DYNAMIC_STORAGE_BIT) {}

            using Buffer::Bind;
            void Bind(int bindBase) const { this->Bind(GL_UNIFORM_BUFFER, bindBase); }
        };

        using DynamicVertexBuffer = UniformBuffer;
        using DynamicIndexBuffer = DynamicVertexBuffer;

        class VertexBuffer : public Buffer
        {
        public:
            VertexBuffer() : Buffer() {}
            VertexBuffer(const void *data, size_t size) : Buffer(data, size, 0) {}
            VertexBuffer(size_t size) : Buffer(size, 0) {}
            template<typename T, std::size_t Size = sizeof T>
            VertexBuffer(const T &data) : Buffer(&data, Size, 0) {}
            template<typename T, std::size_t Size = sizeof T>
            VertexBuffer(const std::vector<T> &data) : Buffer(data.data(), data.size() * Size, 0) {}
        };

        using IndexBuffer = VertexBuffer;
    }
}
