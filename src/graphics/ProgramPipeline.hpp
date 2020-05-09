#pragma once

#include "graphics/GLObject.hpp"

#include "graphics/Shader.hpp"

namespace RIS
{
    namespace Graphics
    {
        class ProgramPipeline : public GLObject
        {
        public:
            ProgramPipeline();
            ~ProgramPipeline();

            ProgramPipeline(const ProgramPipeline &) = delete;
            ProgramPipeline& operator=(const ProgramPipeline &) = delete;

            ProgramPipeline(ProgramPipeline &&);
            ProgramPipeline& operator=(ProgramPipeline &&);

            void SetShader(const Shader &shader);
            void Use();

        };
    }
}
