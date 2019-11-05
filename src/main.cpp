#define UNICODE
#include <Windows.h>

#include <glbinding/gl45core/gl.h>
#include <glbinding/glbinding.h>

#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

using namespace gl;

int main(int argc, char *argv[])
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

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
    cout << version << endl;

    gl::glClearColor(0.392f, 0.584f, 0.929f, 1.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
