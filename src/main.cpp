#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "world.h"

int main()
{
    World world = World();

    renderer scene(world, 800, 600);
    scene.render(world);

    return 0;
}