#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "ray.h"

class Camera
{
public:
    float vfov, focal_length, aspect_ratio, viewport_width, viewport_height;
    float pixelWidth, pixelHeight;

    glm::vec3 pixelX, pixelY;
    int image_height, image_width;


    glm::vec3 position, focal_center;
    
    Camera() =  default;
    Camera(float focal_length, float ar, float vfov, int image_height);

    void set_pos(glm::vec3 pos);

    ray getRay(int width, int height);
};

using namespace std;

