#include "gui.h"

GUI::GUI(World &world, renderer &renderer) : world(world), currRenderer(renderer)
{
    screenHeight = currRenderer.height;
    screenWidth = currRenderer.width;
    setup();
}

void GUI::setup()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable keyboard controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad controls 

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(currRenderer.window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
}

// Start the Dear ImGui frame
void GUI::initFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

// draw all the UI elements
void GUI::renderUI()
{
    
}

void GUI::cleanup()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}