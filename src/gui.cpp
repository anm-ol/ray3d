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
    ImGui::Begin("Debug");
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::Text("Frame time: %.3f ms", 1000.0f / io.Framerate);
    ImGui::Text("Display size: %.0f x %.0f", io.DisplaySize.x, io.DisplaySize.y);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::cleanup()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}