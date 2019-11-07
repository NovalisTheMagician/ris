#define UNICODE
#include <Windows.h>

#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>

//#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

using namespace gl;

int main(int argc, char *argv[])
{
    //FreeConsole();

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SRGB_CAPABLE, true);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
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
    //glEnable(GL_TEXTURE);

    gl::glClearColor(0.392f, 0.584f, 0.929f, 1.0f);

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
