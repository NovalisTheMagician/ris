#include "graphics/VertexArray.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace RIS
{
    namespace Graphics
    {
        VertexArray::VertexArray()
            : GLObject(0)
        {
            glCreateVertexArrays(1, &id);
        }

        VertexArray::~VertexArray()
        {
            glDeleteVertexArrays(1, &id);
        }

        VertexArray::VertexArray(VertexArray &&other)
        {
            std::swap(id, other.id);
        }

        VertexArray& VertexArray::operator=(VertexArray &&other)
        {
            std::swap(id, other.id);
            return *this;
        }

        void VertexArray::SetAttribFormat(int attrib, int numComponents, GLenum type, std::size_t offset, int bindingPoint)
        {
            glEnableVertexArrayAttrib(id, attrib);
            glVertexArrayAttribFormat(id, attrib, numComponents, type, GL_FALSE, offset);
            glVertexArrayAttribBinding(id, attrib, bindingPoint);
        }

        void VertexArray::Bind()
        {
            glBindVertexArray(id);
        }

        void VertexArray::SetVertexBuffer(const Buffer &buffer, int bindingPoint, size_t stride, size_t offset)
        {
            glVertexArrayVertexBuffer(id, bindingPoint, buffer.GetId(), offset, stride);
        }

        void VertexArray::SetIndexBuffer(const Buffer &buffer)
        {
            glVertexArrayElementBuffer(id, buffer.GetId());
        }
    }
}
