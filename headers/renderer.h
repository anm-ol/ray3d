#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "world.h"

class renderer
{
public:
    unsigned int VBO, VAO, texture;
    int texWidth, texHeight, nrChannels;
    int width, height;
    float aspect_ratio;

    float vertices[18] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f    
    };


    GLFWwindow* window;
    Shader shader;

    renderer(World& world, int width, int height);
    void init_window();
    void render(World& world);
    void setupFrame();
    void updateFrame(std::vector<glm::vec4>& pixels);

    void processInput(World& world);
    // static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};