#pragma once

#include "GLObject.hpp"

#include <stdexcept>

#include <glbinding/gl46/gl.h>
#include <unordered_map>
#include <vector>

namespace RIS
{
    template<typename T>
    class Buffer : public GLObject
    {
    private:
        Buffer(const T &data, gl::GLenum usage)
            : elementSize(sizeof(T))
        {
            gl::glCreateBuffers(1, &id);
            gl::glNamedBufferData(id, elementSize, reinterpret_cast<const T*>(&data), usage);
        };

        Buffer(const std::vector<T> &data, gl::GLenum usage)
            : elementSize(sizeof(T))
        {
            gl::glCreateBuffers(1, &id);
            std::size_t size = data.size() * elementSize;
            gl::glNamedBufferData(id, size, data.data(), usage);
        };

        Buffer(std::size_t numElements, gl::GLenum usage)
            : elementSize(sizeof(T))
        {
            gl::glCreateBuffers(1, &id);
            gl::glNamedBufferData(id, numElements * elementSize, nullptr, usage);
        };

        Buffer(const T &data, gl::BufferStorageMask usage)
            : elementSize(sizeof(T))
        {
            gl::glCreateBuffers(1, &id);
            gl::glNamedBufferStorage(id, elementSize, reinterpret_cast<const T*>(&data), usage);
        };

        Buffer(const std::vector<T> &data, gl::BufferStorageMask usage)
            : elementSize(sizeof(T))
        {
            gl::glCreateBuffers(1, &id);
            std::size_t size = data.size() * elementSize;
            gl::glNamedBufferStorage(id, size, data.data(), usage);
        };

        Buffer(std::size_t numElements, gl::BufferStorageMask usage)
            : elementSize(sizeof(T))
        {
            gl::glCreateBuffers(1, &id);
            gl::glNamedBufferData(id, numElements * elementSize, nullptr, usage);
        };

    public:
        Buffer() : GLObject(0) {};
        ~Buffer() { gl::glDeleteBuffers(1, &id); };

        Buffer(const Buffer &) = delete;
        Buffer& operator=(const Buffer &) = delete;

        Buffer(Buffer &&other) 
        {
            id = other.id;
            other.id = 0;
            elementSize = other.elementSize;
        };

        Buffer& operator=(Buffer &&other)
        {
            id = other.id;
            other.id = 0;
            elementSize = other.elementSize;
            return *this;
        };

        void UpdateData(const std::vector<T> &data, std::size_t offset = 0)
        {
            std::size_t size = data.size() * elementSize;
            glNamedBufferSubData(id, offset, size, data.data());
        };

        void UpdateData(const T &data, std::size_t offset = 0)
        {
            glNamedBufferSubData(id, offset, elementSize, reinterpret_cast<const T*>(&data));
        };

        void Bind(gl::GLenum target, int bindBase) { gl::glBindBufferBase(target, bindBase, id); };

        std::size_t GetElementSize() const { return elementSize; };

    public:
        static Buffer<T> Create(const T &data, gl::GLenum usage) { return Buffer<T>(data, usage); };
        static Buffer<T> Create(const std::vector<T> &data, gl::GLenum usage) { return Buffer<T>(data, usage); };
        static Buffer<T> Create(std::size_t numElements, gl::GLenum usage) { return Buffer<T>(numElements, usage); };

        static Buffer<T> CreateImmutable(const T &data, gl::BufferStorageMask usage) { return Buffer<T>(data, usage); };
        static Buffer<T> CreateImmutable(const std::vector<T> &data, gl::BufferStorageMask usage) { return Buffer<T>(data, usage); };
        static Buffer<T> CreateImmutable(std::size_t numElements, gl::BufferStorageMask usage) { return Buffer<T>(numElements, usage); };

    private:
        std::size_t elementSize;

    };
}
