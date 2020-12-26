#pragma once

#include "graphics/Buffer.hpp"
#include <glad/glad.h>

#include <gsl/span>

namespace RIS::Graphics
{
    template<typename T>
    class StreamingBuffer
    {
    public:
        using Element_Type = T;

        StreamingBuffer(size_t numElem = 1);
        ~StreamingBuffer();

        StreamingBuffer(const StreamingBuffer&) = delete;
        StreamingBuffer& operator=(const StreamingBuffer&) = delete;
        StreamingBuffer(StreamingBuffer&&) = delete;
        StreamingBuffer& operator=(StreamingBuffer&&) = delete;

        void operator=(const T &data);
        T& operator[](size_t index);
        T* operator->();

        void Bind(GLenum target, int bindBase);
        Buffer& GetBuffer();

    private:
        Buffer buffer;
        gsl::span<T> view;

    };

    template<typename T>
    StreamingBuffer<T>::StreamingBuffer(size_t numElem)
        : buffer(numElem * sizeof T, GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT)
        , view(static_cast<T*>(buffer.Map(GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)), numElem)
    {}

    template<typename T>
    StreamingBuffer<T>::~StreamingBuffer()
    {
        buffer.UnMap();
    }

    template<typename T>
    T& StreamingBuffer<T>::operator[](size_t index)
    {
        return view[index];
    }

    template<typename T>
    void StreamingBuffer<T>::operator=(const T &data)
    {
        view[0] = data;
        //return *this;
    }

    template<typename T>
    T* StreamingBuffer<T>::operator->()
    {
        return &view[0];
    }

    template<typename T>
    void StreamingBuffer<T>::Bind(GLenum target, int bindBase)
    {
        buffer.Bind(target, bindBase);
    }

    template<typename T>
    Buffer& StreamingBuffer<T>::GetBuffer()
    {
        return buffer;
    }
}
