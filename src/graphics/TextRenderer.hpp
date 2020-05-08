#pragma once

#include "graphics/Shader.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Font.hpp"
#include "graphics/Sampler.hpp"
#include "graphics/ProgramPipeline.hpp"
#include "graphics/VertexArray.hpp"

#include <memory>
#include <string>

#include <glm/glm.hpp>

namespace RIS
{
    namespace Graphics
    {
        class TextRenderer
        {
        public:
            TextRenderer();
            ~TextRenderer() = default;
            TextRenderer(const TextRenderer&) = delete;
            TextRenderer& operator=(const TextRenderer&) = delete;
            TextRenderer(TextRenderer&&) = default;
            TextRenderer& operator=(TextRenderer&&) = default;

            void DrawString(const std::string &string, const Font &font, float size, const glm::vec2 &position);

        private:
            static const int MAX_STRING_LEN;

        private:
            Buffer vertexBuffer;
            Sampler sampler;
            std::shared_ptr<Shader> vertexShader, fragmentShader;
            ProgramPipeline pipeline;
            VertexArray vertexLayout;

        };
    }
}