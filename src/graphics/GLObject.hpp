#pragma once

#include <glad/glad.h>

namespace RIS::Graphics
{
    class GLObject
    {
    public:
        GLObject() : id(0) {};
        GLObject(GLuint id) : id(id) {};
        virtual ~GLObject() = default;

        GLuint GetId() const { return id; };

        bool operator==(const GLObject &other) const { return id == other.id; };
        bool operator!=(const GLObject &other) const { return !(*this == other); };

    protected:
        GLuint id;

    };
}
