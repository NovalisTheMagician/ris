#pragma once

#include "RisExcept.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <vector>
#include <memory>

#include "graphics/VertexTypes.hpp"

#include "graphics/Buffer.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Framebuffer.hpp"
#include "graphics/Sampler.hpp"
#include "graphics/VertexArray.hpp"
#include "graphics/Shader.hpp"
#include "graphics/ProgramPipeline.hpp"

#include "graphics/Mesh.hpp"
#include "graphics/Model.hpp"
#include "graphics/Font.hpp"

namespace RIS
{
    namespace Graphics
    {
        struct RendererException : public RISException
        {
            RendererException(const std::string &reason) : RISException(reason) {}
        };
        
        class Renderer
        {
        public:
            Renderer();
            ~Renderer() = default;
            Renderer(const Renderer &) = delete;
            Renderer(Renderer &&) = delete;
            Renderer &operator=(const Renderer &) = default;
            Renderer &operator=(Renderer &&) = default;

            void PostInit();

            void ClearCacheTextures();
            void ClearCacheMeshes();
            void ClearCacheModels();
            void ClearCacheShaders();
            void ClearCache();

            void Resize(int width, int height);

        private:
            std::unordered_map<std::string, std::shared_ptr<Texture>> loadedTextures;
            std::unordered_map<std::string, std::shared_ptr<Mesh>> loadedMeshes;
            std::unordered_map<std::string, std::shared_ptr<Model>> loadedModels;
            std::unordered_map<std::string, std::shared_ptr<Shader>> loadedShaders;
            std::unordered_map<std::string, std::shared_ptr<Font>> loadedFonts;

            bool useAmdFix = false;

        };
    }
}
