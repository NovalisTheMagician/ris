#include <Windows.h>

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include "common/Args.hpp"

using std::string;
using std::cout;
using std::endl;

using namespace gl46core;
using namespace RIS;

int main(int argc, char *argv[])
{
    FreeConsole();

    Args args(argc, argv);

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SRGB_CAPABLE, true);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "RIS", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glbinding::initialize(glfwGetProcAddress);

    string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    string shaderVer = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    cout << version << endl;
    cout << renderer << endl;
    cout << shaderVer << endl;
    cout << vendor << endl;

    glEnable(GL_FRAMEBUFFER_SRGB);
    

    glClearColor(0.392f, 0.584f, 0.929f, 1.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}
