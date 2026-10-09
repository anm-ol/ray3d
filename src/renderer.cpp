#include "renderer.h"
#include <iostream>
// #include <vector>

renderer::renderer(World& world, int width, int height): world(world), width(width), height(height)
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

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
        std::cout << "fb " << width << "x" << height
          << " vp " << vw << "x" << vh << "\n";
        glViewport((width - vw) / 2, (height - vh) / 2, vw, vh);
    };

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    glfwSetCursorPosCallback(window, mouseCallback);
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
        auto t0 = std::chrono::high_resolution_clock::now();
        world.sampleRays(accumulate);

        updateFrame(world.pixels);
        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration<float,std::milli>(t1-t0).count() << " ms\n";

        auto camPos = world.cam.position;
        processInput(world);
        accumulate = (camPos == world.cam.position);
        
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    
    glfwTerminate();
}
void renderer::processInput(World& world)
{   
    auto cameraPos = world.cam.position;
    const float cameraSpeed = 0.2f; // adjust accordingly
    auto cameraFront = world.cam.frontDir;
    auto cameraUp = world.cam.upDir;
    auto worldUp = glm::vec3(0, 1, 0);

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        world.cam.setPosition(cameraPos + cameraSpeed * cameraFront);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        world.cam.setPosition(cameraPos - cameraSpeed * cameraFront);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        world.cam.setPosition(cameraPos - glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        world.cam.setPosition(cameraPos + glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        world.cam.setPosition(cameraPos + cameraSpeed * worldUp);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        world.cam.setPosition(cameraPos - cameraSpeed * worldUp);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    static bool firstMouse = true;
    renderer* windowUser = static_cast<renderer*>(glfwGetWindowUserPointer(window));
    windowUser->accumulate = false;

    if (firstMouse)
    {
        windowUser->lastX = xpos;
        windowUser->lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - windowUser->lastX;
    float yoffset = windowUser->lastY - ypos; 
    windowUser->lastX = xpos;
    windowUser->lastY = ypos;

    auto& camera = windowUser->world.cam;
    camera.processCameraMovement(xoffset, yoffset);
}