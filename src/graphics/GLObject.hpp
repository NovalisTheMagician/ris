#pragma once

#include <glbinding/gl46/gl.h>

namespace RIS
{
    namespace Graphics
    {
        class GLObject
        {
        public:
            GLObject() : id(0) {};
            GLObject(gl::GLuint id) : id(id) {};
            virtual ~GLObject() = default;

            gl::GLuint GetId() const { return id; };

            bool operator==(const GLObject &other) const { return id == other.id; };
            bool operator!=(const GLObject &other) const { return !(*this == other); };

        protected:
            gl::GLuint id;

        };
    }
}
