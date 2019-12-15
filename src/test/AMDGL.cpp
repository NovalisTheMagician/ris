#include <Windows.h>

#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

#include <array>
#include <string>
#include <iostream>

using namespace gl46core;

struct Vertex
{
    float pos[2];
};

const char *vertexShaderSrc =   "#version 460 core\n"
                                "layout(location=0) in vec2 pos;\n"
                                "out gl_PerVertex {"
                                "   vec4 gl_Position;"
                                "};"
                                "void main() {\n"
                                "   gl_Position = vec4(pos, 0, 1);\n"
                                "}\n";

const char *fragmentShaderSrc = "#version 460 core\n"
                                "layout(location=0) out vec4 fragColor;\n"
                                "void main() {\n"
                                "   fragColor = vec4(0, 1, 0, 1);\n"
                                "}\n";

const uint32_t vertexShaderSPIRV[] = 
{
    0x07230203,0x00010000,0x00080008,0x00000018,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0007000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000a,0x0000000f,0x00030003,
	0x00000002,0x000001cc,0x00040005,0x00000004,0x6e69616d,0x00000000,0x00060005,0x00000008,
	0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000008,0x00000000,0x505f6c67,
	0x7469736f,0x006e6f69,0x00030005,0x0000000a,0x00000000,0x00030005,0x0000000f,0x00736f70,
	0x00050048,0x00000008,0x00000000,0x0000000b,0x00000000,0x00030047,0x00000008,0x00000002,
	0x00040047,0x0000000f,0x0000001e,0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,
	0x00000002,0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,
	0x0003001e,0x00000008,0x00000007,0x00040020,0x00000009,0x00000003,0x00000008,0x0004003b,
	0x00000009,0x0000000a,0x00000003,0x00040015,0x0000000b,0x00000020,0x00000001,0x0004002b,
	0x0000000b,0x0000000c,0x00000000,0x00040017,0x0000000d,0x00000006,0x00000002,0x00040020,
	0x0000000e,0x00000001,0x0000000d,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x0004002b,
	0x00000006,0x00000011,0x00000000,0x0004002b,0x00000006,0x00000012,0x3f800000,0x00040020,
	0x00000016,0x00000003,0x00000007,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,
	0x000200f8,0x00000005,0x0004003d,0x0000000d,0x00000010,0x0000000f,0x00050051,0x00000006,
	0x00000013,0x00000010,0x00000000,0x00050051,0x00000006,0x00000014,0x00000010,0x00000001,
	0x00070050,0x00000007,0x00000015,0x00000013,0x00000014,0x00000011,0x00000012,0x00050041,
	0x00000016,0x00000017,0x0000000a,0x0000000c,0x0003003e,0x00000017,0x00000015,0x000100fd,
	0x00010038
};

const uint32_t fragmentShaderSPIRV[] = 
{
    0x07230203,0x00010000,0x00080008,0x0000000d,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0006000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x00030010,0x00000004,
	0x00000007,0x00030003,0x00000002,0x000001cc,0x00040005,0x00000004,0x6e69616d,0x00000000,
	0x00050005,0x00000009,0x67617266,0x6f6c6f43,0x00000072,0x00040047,0x00000009,0x0000001e,
	0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
	0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
	0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x0004002b,0x00000006,0x0000000a,
	0x00000000,0x0004002b,0x00000006,0x0000000b,0x3f800000,0x0007002c,0x00000007,0x0000000c,
	0x0000000a,0x0000000b,0x0000000a,0x0000000b,0x00050036,0x00000002,0x00000004,0x00000000,
	0x00000003,0x000200f8,0x00000005,0x0003003e,0x00000009,0x0000000c,0x000100fd,0x00010038
};

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int show)
{
    int argc = __argc;
    char **argv = __argv;
#else
int main(int argc, char *argv[])
{
#endif

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); 

    GLFWwindow *window = glfwCreateWindow(800, 600, "AMD Test", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    glbinding::initialize(glfwGetProcAddress);

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

    std::array<Vertex, 6> vertices;
    vertices[0] = {{ 0.5f,  0.5f}};
    vertices[1] = {{-0.5f,  0.5f}};
    vertices[2] = {{-0.5f, -0.5f}};
    vertices[3] = {{ 0.5f, -0.5f}};
    vertices[4] = {{ 0.5f,  0.5f}};
    vertices[5] = {{-0.5f, -0.5f}};

    GLuint buffer;
    glCreateBuffers(1, &buffer);
    glNamedBufferStorage(buffer, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

    GLuint vertexArray;
    glCreateVertexArrays(1, &vertexArray);
    glVertexArrayVertexBuffer(vertexArray, 0, buffer, 0, sizeof(Vertex));
    glVertexArrayAttribFormat(vertexArray, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(vertexArray, 0);
    glVertexArrayAttribBinding(vertexArray, 0, 0);
    glBindVertexArray(vertexArray);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

#if 0
    glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
#else
    glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, vertexShaderSPIRV, sizeof(vertexShaderSPIRV));
    glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, fragmentShaderSPIRV, sizeof(fragmentShaderSPIRV));

    glSpecializeShader(vertexShader, "main", 0, nullptr, nullptr);
    glSpecializeShader(fragmentShader, "main", 0, nullptr, nullptr);
#endif

#if 1
    GLuint vertexProgram = glCreateProgram();
    GLuint fragmentProgram = glCreateProgram();

    glProgramParameteri(vertexProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glProgramParameteri(fragmentProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);

    glAttachShader(vertexProgram, vertexShader);
    glAttachShader(fragmentProgram, fragmentShader);

    glLinkProgram(vertexProgram);
    glLinkProgram(fragmentProgram);

    glDetachShader(vertexProgram, vertexShader);
    glDetachShader(fragmentProgram, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint pipeline;
    glCreateProgramPipelines(1, &pipeline);
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertexProgram);
    glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragmentProgram);
    glBindProgramPipeline(pipeline);
#else
    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(program);
#endif

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
    }

}