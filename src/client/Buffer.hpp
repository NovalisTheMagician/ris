#pragma once

#include "GLObject.hpp"

#include <stdexcept>

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

#include <unordered_map>
#include <vector>

namespace RIS
{
    class Buffer : public GLObject
    {
    private:
        Buffer(const void *data, size_t size, gl::BufferStorageMask usage);
        Buffer(size_t size, gl::BufferStorageMask usage);

    public:
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

        size_t GetSize() const;

    public:
        static Buffer Create(const void *data, size_t size, gl::BufferStorageMask usage);
        static Buffer Create(size_t size, gl::BufferStorageMask usage);

        template<typename T>
        static Buffer Create(const T &data, gl::BufferStorageMask usage);
        template<typename T>
        static Buffer Create(const std::vector<T> &data, gl::BufferStorageMask usage);

    private:
        const size_t maxSize;

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
    Buffer Buffer::Create(const T &data, gl::BufferStorageMask usage)
    {
        return Buffer(&data, sizeof T, usage);
    }

    template<typename T>
    Buffer Buffer::Create(const std::vector<T> &data, gl::BufferStorageMask usage)
    {
        return Buffer(data.data(), data.size() * sizeof T, usage);
    }
}
