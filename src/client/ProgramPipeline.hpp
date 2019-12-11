#pragma once

#include "GLObject.hpp"

#include <glbinding/gl46core/gl.h>

#include "Shader.hpp"

namespace RIS
{
    class ProgramPipeline : public GLObject
    {
    private:
        ProgramPipeline(gl::GLuint id);

    public:
        ProgramPipeline();
        ~ProgramPipeline();

        ProgramPipeline(const ProgramPipeline &) = delete;
        ProgramPipeline& operator=(const ProgramPipeline &) = delete;

        ProgramPipeline(ProgramPipeline &&);
        ProgramPipeline& operator=(ProgramPipeline &&);

        void SetShader(const Shader &shader);
        void Use();

    public:
        static ProgramPipeline Create();

    };
}
