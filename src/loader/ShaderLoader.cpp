#include "loader/ShaderLoader.hpp"

#include "loader/TextLoader.hpp"

#include "graphics/Shader.hpp"
#include "graphics/ShaderSourceBuilder.hpp"

namespace RIS::Loader
{
    template<>
    std::shared_ptr<Graphics::Shader> Load<Graphics::Shader>(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
    {
        GLenum shaderType;
        auto shaderParam = std::any_cast<Graphics::ShaderType>(param);
        if(shaderParam == Graphics::ShaderType::VERTEX)
            shaderType = GL_VERTEX_SHADER;
        else if(shaderParam == Graphics::ShaderType::FRAGMENT)
            shaderType = GL_FRAGMENT_SHADER;

        std::string path = std::filesystem::path(name).remove_filename().generic_string();

        Graphics::ShaderSourceBuilder builder;

        std::string shaderSrc(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        std::string shaderSrcProcessed = builder.BuildSource(shaderSrc, [&resourcePack, &path](const std::string &fileName)
        {
            std::string file = path + fileName;
            return *Load<std::string>(resourcePack.Read(file), file, {}, resourcePack);
        });

        return std::make_shared<Graphics::Shader>(shaderSrcProcessed, shaderType);        
    }
}
