#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer.h"

float vertices[] = {
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f    
};


int main()
{
    renderer scene(800, 600);
    scene.render();

    return 0;
}

