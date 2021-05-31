#pragma once

#include "graphics/Buffer.hpp"
#include "graphics/Sampler.hpp"
#include "graphics/Shader.hpp"
#include "graphics/ProgramPipeline.hpp"
#include "graphics/VertexArray.hpp"
#include "graphics/VertexTypes.hpp"
#include "graphics/Font.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Colors.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <string_view>

#include "loader/ResourcePack.hpp"

namespace RIS::Graphics
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

        void Begin();
        void End();
        void SetViewport(float width, float height, bool flip = false);
        void SetTextProperty(float buffer, float gamma);

        void DrawTexture(const Texture &texture, const glm::vec2 &position = {}, const glm::vec2 &size = {1, 1}, const glm::vec4 &tint = Colors::White);
        void DrawRect(const glm::vec2 &position = {}, const glm::vec2 &size = {1, 1}, const glm::vec4 &tint = Colors::White);
        void DrawString(const std::string_view string, const Font &font, float size, const glm::vec2 &position = {}, const glm::vec4 &tint = Colors::White);

    private:
        static constexpr int MAX_STRING_LEN = 1024;

        struct WorldBufferData
        {
            glm::mat4 world;
            glm::vec4 tint;
        };

        struct TextPropertyData
        {
            float buffer;
            float gamma;
        };

    private:
        DynamicVertexBuffer vertexSpriteBuffer, vertexTextBuffer;
        Sampler sampler;
        Shader::Ptr vertexShader, fragmentSpriteShader, fragmentTextShader;
        ProgramPipeline pipeline;
        VertexArray vertexLayout;
        UniformBuffer viewProjectionBuffer, worldBuffer, textPropertyBuffer;
        Texture white;

    };
}
