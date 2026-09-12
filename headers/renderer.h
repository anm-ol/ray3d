#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class renderer
{
public:
    unsigned int VBO;
    int width, height;
    GLFWwindow* window;

    renderer(int width, int height);
    void init_window();
    void render();

    void processInput();
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

};