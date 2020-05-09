#include "graphics/Renderer.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gli/gli.hpp>

#include <iostream>
#include <vector>
#include <string>

#include <experimental/unordered_map>

#include "misc/Logger.hpp"

#include "graphics/VertexTypes.hpp"

#include "RIS.hpp"

#include "loader/Loader.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

using namespace std::literals;

// Windows "hack" to force some laptops to use the highperformance GPU
// some intel integrated gpus don't support opengl 4.6
// I might, in the future, reduce the required opengl version
#ifdef _WIN32
extern "C" 
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace RIS
{
    namespace Graphics
    {
        Renderer::Renderer()
        {
            auto &log = Logger::Instance();

            if(!gladLoadGL())
            {
                //throw
            }

            if(!GLAD_GL_VERSION_4_6)
            {
                //throw
            }

            if(!GLAD_GL_ARB_shading_language_include)
            {
                //throw
            }

#ifdef _DEBUG
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

            auto dbgCallback = [](GLenum source, 
                                GLenum type, 
                                GLuint id, 
                                GLenum severity, 
                                GLsizei length, 
                                const GLchar *message, 
                                const void *userParam) -> void
                {
                    auto const src_str = [source]() {
                        switch (source)
                        {
                        case GL_DEBUG_SOURCE_API: return "API";
                        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
                        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
                        case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
                        case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
                        case GL_DEBUG_SOURCE_OTHER: return "OTHER";
                        default: return "";
                        }
                    }();

                    auto const type_str = [type]() {
                        switch (type)
                        {
                        case GL_DEBUG_TYPE_ERROR: return "ERROR";
                        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
                        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
                        case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
                        case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
                        case GL_DEBUG_TYPE_MARKER: return "MARKER";
                        case GL_DEBUG_TYPE_OTHER: return "OTHER";
                        default: return "";
                        }
                    }();

                    auto const severity_str = [severity]() {
                        switch (severity) {
                        case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
                        case GL_DEBUG_SEVERITY_LOW: return "LOW";
                        case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
                        case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
                        default: return "";
                        }
                    }();

                    std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << std::endl;
                };

            glDebugMessageCallback(static_cast<GLDEBUGPROC>(dbgCallback), nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
            std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
            std::string shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
            std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

            if(vendor.find("ATI") != std::string::npos || vendor.find("ati") != std::string::npos)
                useAmdFix = true;

            Config &config = GetConfig();

            if(const_cast<const Config&>(config).GetValue("g_ignoreamdfix", false))
                useAmdFix = false;

            log.Info("Using OpenGL version " + version + " from " + vendor + " with shaderversion " + shaderVersion + " on " + renderer);

            int width = config.GetValue("r_width", 800);
            int height = config.GetValue("r_height", 600);
            Resize(width, height);

            loadedTextures.insert({"__DEFAULT_TEX", std::make_shared<Texture>(glm::vec4(1, 1, 1, 1))});
            loadedTextures.insert({"__MISSING_TEX", std::make_shared<Texture>(glm::vec4(1, 0, 1, 1))});

        }

        void Renderer::PostInit()
        {
            auto &loader = GetSystems().GetLoader();

            loader.RegisterLoadFunction<Texture>([this](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                if(loadedTextures.count(name) > 0)
                {
                    return loadedTextures.at(name);
                }
                auto texture = std::make_shared<Texture>(data);
                if(cache)
                    loadedTextures.insert_or_assign(name, texture);
                return texture;
            }, [this]()
            {
                return loadedTextures.at("__MISSING_TEX");
            });

            loader.RegisterLoadFunction<Shader>([this](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                if(loadedShaders.count(name) > 0)
                {
                    return loadedShaders.at(name);
                }

                GLenum shaderType;
                auto shaderParam = std::any_cast<ShaderType>(param);
                if(shaderParam == ShaderType::VERTEX)
                    shaderType = GL_VERTEX_SHADER;
                else if(shaderParam == ShaderType::FRAGMENT)
                    shaderType = GL_FRAGMENT_SHADER;

                std::shared_ptr<Shader> shader;
                if(useAmdFix)
                {
                    shader = std::make_shared<TextShader>(data, shaderType);
                }
                else
                {
                    shader = std::make_shared<BinaryShader>(data, shaderType);
                }
                if(cache)
                    loadedShaders.insert_or_assign(name, shader);
                return shader;
            });

            loader.RegisterLoadFunction<Image>([](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                return std::make_shared<Image>(data);
            });

            loader.RegisterLoadFunction<Font>([this, &loader](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                if(loadedFonts.count(name) > 0)
                {
                    return loadedFonts.at(name);
                }

                std::string fontStr(reinterpret_cast<const char*>(data.data()), data.size());

                rapidjson::Document fontJson;
                rapidjson::ParseResult res = fontJson.Parse(fontStr.c_str()); 
                if(res.IsError())
                {
                    std::string errorMsg = rapidjson::GetParseError_En(res.Code());
                    Logger::Instance().Error("Failed to parse font ("s + name + "): "s + errorMsg + "(" + std::to_string(res.Offset()) + ")");
                    throw std::runtime_error("font error"); //make better error
                }

                float ascender = fontJson["ascender"].GetFloat();
                float bitmapWidth = fontJson["bitmap_width"].GetInt();
                float bitmapHeight = fontJson["bitmap_height"].GetInt();
                float descender = fontJson["descender"].GetFloat();
                float height = fontJson["height"].GetFloat();
                float maxAdvance = fontJson["max_advance"].GetFloat();
                std::string fontName = fontJson["name"].GetString();
                float size = fontJson["size"].GetFloat();
                float spaceAdvance = fontJson["space_advance"].GetFloat();

                std::string fontTexturePath = "fonts/" + fontName + ".dds";
                auto fontTexture = loader.Load<Texture>(fontTexturePath);

                std::unordered_map<char, Glyph> glyphs;

                const rapidjson::Value &glyphData = fontJson["glyph_data"];
                for (auto itr = glyphData.MemberBegin(); itr != glyphData.MemberEnd(); ++itr)
                {
                    const rapidjson::Value &glyphJson = itr->value;
                    Glyph glyph = { 0 };
                    glyph.advanceX = glyphJson["advance_x"].GetFloat();
                    glyph.bboxHeight = glyphJson["bbox_height"].GetFloat();
                    glyph.bboxWidth = glyphJson["bbox_width"].GetFloat();
                    glyph.bearingX = glyphJson["bearing_x"].GetFloat();
                    glyph.bearingY = glyphJson["bearing_y"].GetFloat();
                    glyph.charCode = glyphJson["charcode"].GetString()[0];
                    glyph.s0 = glyphJson["s0"].GetFloat();
                    glyph.t0 = glyphJson["t0"].GetFloat();
                    glyph.s1 = glyphJson["s1"].GetFloat();
                    glyph.t1 = glyphJson["t1"].GetFloat();

                    const rapidjson::Value &kerningsJson = glyphJson["kernings"];
                    for(auto kItr = kerningsJson.MemberBegin(); kItr != kerningsJson.MemberEnd(); ++kItr)
                    {
                        char c = kItr->name.GetString()[0];
                        float val = kItr->value.GetFloat();
                        glyph.kernings.insert({ c, val });
                    }

                    glyphs.insert({ glyph.charCode, glyph });
                }

                auto font = std::make_shared<Font>(ascender, descender, height, maxAdvance, fontName, size, spaceAdvance, glyphs, fontTexture);
                if(cache)
                    loadedFonts.insert_or_assign(name, font);
                return font;
            });
        }

        void Renderer::ClearCacheTextures()
        {
            loadedTextures.clear();
        }

        void Renderer::ClearCacheMeshes()
        {
            loadedMeshes.clear();
        }

        void Renderer::ClearCacheModels()
        {
            loadedModels.clear();
        }

        void Renderer::ClearCacheShaders()
        {
            loadedShaders.clear();
        }

        void Renderer::ClearCache()
        {
            ClearCacheTextures();
            ClearCacheMeshes();
            ClearCacheModels();
            ClearCacheShaders();
        }

        void Renderer::Resize(int width, int height)
        {
            glViewport(0, 0, width, height);
        }
    }
}
