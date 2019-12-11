#pragma once

#include <glbinding/gl46/gl.h>

namespace RIS
{
    class GLObject
    {
    public:
        GLObject() : id(0) {};
        GLObject(gl::GLuint id) : id(id) {};
        virtual ~GLObject() = default;

        gl::GLuint GetId() const { return id; };

    protected:
        gl::GLuint id;

    };
}
