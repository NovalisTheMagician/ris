#pragma once

#include "graphics/GLRenderer.hpp"

namespace RIS
{
    class GLRenderer;

    class GL3DRenderer
    {
    public:
        GL3DRenderer(GLRenderer &renderer);
        ~GL3DRenderer() = default;
        GL3DRenderer(const GL3DRenderer&) = delete;
        GL3DRenderer& operator=(const GL3DRenderer&) = delete;
        GL3DRenderer(GL3DRenderer&&) = default;
        GL3DRenderer& operator=(GL3DRenderer&&) = default;

        void Setup();
        void LoadShaders();

        ResourceId LoadMesh(const std::string &name);
        void DestroyMesh(ResourceId meshId);

        ResourceId LoadModel(const std::string &name);
        void DestroyModel(ResourceId modelId);

        void SetTexture(ResourceId textureId);

        void Begin(const glm::mat4 &viewProjection);
        void End();

        void Draw(ResourceId modelId, const glm::mat4 &world);

    public:
        static const ResourceId MISSING_MODEL_ID;
        static const ResourceId MISSING_MESH_ID;

    private:
        struct PerFrameMatrices
        {
            glm::mat4 viewProjection;
        };

        struct PerObjectMatrices
        {
            glm::mat4 world;
        };

    private:
        GLRenderer &renderer;

        ResourceId highestUnusedMeshId = 1;
        std::unordered_map<ResourceId, Mesh> meshes;
        std::unordered_map<std::string, ResourceId> loadedMeshes;

        ResourceId highestUnusedModelId = 1;
        std::unordered_map<ResourceId, Model> models;
        std::unordered_map<std::string, ResourceId> loadedModels;

        VertexArray modelVAO;
        Shader staticModelShader, modelUnlitShader;
        Buffer perFrameBuffer, perObjectBuffer, skeletonBuffer;
        PerFrameMatrices perFrameData;
        PerObjectMatrices perObjectData;

        Sampler defaultSampler;

    };
}
