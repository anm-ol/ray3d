#pragma once 

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "world.h"
#include "renderer.h"

class GUI
{
public:
    float screenHeight, screenWidth;
    World& world;
    renderer& currRenderer;

    GUI(World& world, renderer& renderer);
    void setup();
    void initFrame();
    void renderUI();
    void cleanup();
};