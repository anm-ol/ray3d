#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "ray.h"

class Camera
{
public:
    float vfov, focal_length, aspect_ratio, viewport_width, viewport_height;
    float pixelWidth, pixelHeight;

    float pitch, yaw;

    glm::vec3 pixelX, pixelY;
    int image_height, image_width;


    glm::vec3 position, focal_center;
    glm::vec3 frontDir, upDir;
    
    Camera() =  default;
    Camera(float focal_length, float ar, float vfov, int image_height);

    void setPosition(glm::vec3 pos);
    void setOrientation();
    void processCameraMovement(float xoffset, float yoffset);

    ray getRay(int width, int height);
};
