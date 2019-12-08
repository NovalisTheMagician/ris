#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>

using namespace gl46core;

namespace RIS
{
    ProgramPipeline::ProgramPipeline()
        : pipelineId(0)
    {
    }

    ProgramPipeline::~ProgramPipeline()
    {
        glDeleteProgramPipelines(1, &pipelineId);
    }

    void ProgramPipeline::Create()
    {
        glGenProgramPipelines(1, &pipelineId);
    }

    void ProgramPipeline::SetShader(const Shader &shader, UseProgramStageMask type)
    {
        glUseProgramStages(pipelineId, type, shader.GetId());
    }

    void ProgramPipeline::Use()
    {
        glBindProgramPipeline(pipelineId);
    }

    GLuint ProgramPipeline::GetId() const
    {
        return pipelineId;
    }
}
