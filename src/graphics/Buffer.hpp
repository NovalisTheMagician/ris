#pragma once

#include "graphics/GLObject.hpp"

#include <stdexcept>

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

#include <unordered_map>
#include <vector>

namespace RIS
{
    namespace Graphics
    {
        class Buffer : public GLObject
        {
        public:
            Buffer(const void *data, size_t size, gl::BufferStorageMask usage);
            Buffer(size_t size, gl::BufferStorageMask usage);
            template<typename T>
            Buffer(const T &data, gl::BufferStorageMask usage);
            template<typename T>
            Buffer(const std::vector<T> &data, gl::BufferStorageMask usage);

            Buffer();
            ~Buffer();

            Buffer(const Buffer &) = delete;
            Buffer& operator=(const Buffer &) = delete;

            Buffer(Buffer &&other);
            Buffer& operator=(Buffer &&other);

            void UpdateData(const void *data, size_t size, size_t offset = 0);

            template<typename T>
            void UpdateData(const T &data, size_t offset = 0);
            template<typename T>
            void UpdateData(const std::vector<T> &data, size_t offset = 0);

            void Bind(gl::GLenum target, int bindBase);

            void* Map(gl::BufferAccessMask access, size_t size = 0, size_t offset = 0);
            void UnMap();
            void Flush(size_t size = 0, size_t offset = 0);

            size_t GetSize() const;

        private:
            size_t maxSize;

        };

        template<typename T>
        void Buffer::UpdateData(const T &data, size_t offset)
        {
            UpdateData(&data, sizeof T, offset);
        }

        template<typename T>
        void Buffer::UpdateData(const std::vector<T> &data, size_t offset)
        {
            UpdateData(data.data(), data.size() * sizeof T, offset);
        }

        template<typename T>
        Buffer::Buffer(const T &data, gl::BufferStorageMask usage)
            : Buffer(&data, sizeof T, usage)
        {}

        template<typename T>
        Buffer::Buffer(const std::vector<T> &data, gl::BufferStorageMask usage)
            : Buffer(data.data(), data.size() * sizeof T, usage)
        {}
    }
}
