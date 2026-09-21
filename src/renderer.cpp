#include "renderer.h"
#include <iostream>
// #include <vector>

renderer::renderer(World& world, int width, int height): width(width), height(height)
{
    init_window();
    std::string shaderPath = std::filesystem::current_path().string() + "/shaders/";
	shader = Shader(shaderPath + "vert.glsl", shaderPath + "frag.glsl");

    texHeight = world.cam.image_height;
    texWidth = world.cam.image_width;
    aspect_ratio = world.cam.aspect_ratio;
    nrChannels = 4;
}

void renderer::init_window()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "ray3d", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    } 

    glViewport(0, 0, width, height);
    glfwSetWindowUserPointer(window, this);

    auto framebuffer_size_callback = [](GLFWwindow* window, int width, int height)
    {
        auto* self = static_cast<renderer*>(glfwGetWindowUserPointer(window));
        auto image_ar = self->aspect_ratio;
        float win_ar = (float)height/width;
        int vh, vw;
        if(win_ar < image_ar) {vw = width; vh = width*image_ar;}
        else
        {
            vh = height; vw = height/image_ar;
        }

        glViewport((width - vw) / 2, (height - vh) / 2, vw, vh);
    };
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
}

// Change from reinhard tonemap to something else
void renderer::setupFrame()
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texWidth, texHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenVertexArrays(1, &VAO); 
}

void renderer::updateFrame(std::vector<glm::vec4>& pixels)
{   
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RGBA, GL_FLOAT, pixels.data());

    shader.use();
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader.setInt("uImage", 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

// Eventually want to move all opengl and window stuff out of renderer and maybe rename this
void renderer::render(World& world)
{
    setupFrame();

    // render loop
    while(!glfwWindowShouldClose(window))
    {   
        world.sampleRays();
        updateFrame(world.pixels);

        processInput();
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    
    glfwTerminate();
}
void renderer::processInput()
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}