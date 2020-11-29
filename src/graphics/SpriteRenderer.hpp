#pragma once

#include "graphics/Buffer.hpp"
#include "graphics/Sampler.hpp"
#include "graphics/Shader.hpp"
#include "graphics/ProgramPipeline.hpp"
#include "graphics/VertexArray.hpp"
#include "graphics/VertexTypes.hpp"
#include "graphics/Font.hpp"
#include "graphics/Texture.hpp"

#include <glm/glm.hpp>

#include <memory>

#include "loader/ResourcePack.hpp"

namespace RIS
{
    namespace Graphics
    {
        class SpriteRenderer
        {
        public:
            SpriteRenderer(const Loader::ResourcePack &resourcePack);
            ~SpriteRenderer() = default;
            SpriteRenderer(const SpriteRenderer&) = delete;
            SpriteRenderer& operator=(const SpriteRenderer&) = delete;
            SpriteRenderer(SpriteRenderer&&) = default;
            SpriteRenderer& operator=(SpriteRenderer&&) = default;

            void Begin(float viewWidth, float viewHeight);
            void End();

            void DrawTexture(const Texture &texture, const glm::vec2 &position = {}, const glm::vec2 &size = {1, 1}, const glm::vec4 &tint = {1, 1, 1, 1});
            void DrawRect(const glm::vec2 &position = {}, const glm::vec2 &size = {1, 1}, const glm::vec4 &tint = {1, 1, 1, 1});
            void DrawString(const std::string &string, const Font &font, float size, const glm::vec2 &position = {}, const glm::vec4 &tint = {1, 1, 1, 1});

        private:
            static const int MAX_STRING_LEN;

            struct WorldBufferData
            {
                glm::mat4 world;
                glm::vec4 tint;
            };

        private:
            DynamicVertexBuffer vertexSpriteBuffer, vertexTextBuffer;
            Sampler sampler;
            std::shared_ptr<Shader> vertexShader, fragmentSpriteShader, fragmentTextShader;
            ProgramPipeline pipeline;
            VertexArray vertexLayout;
            UniformBuffer viewProjectionBuffer, worldBuffer;
            Texture white;

        };
    }
}
