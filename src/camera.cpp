#include <glm/glm.hpp>
#include <numbers>
#include "camera.h"
#include <iostream>

#define PI 3.14159
Camera::Camera(float f, float ar, float vfov, int image_height) : focal_length(f), aspect_ratio(ar), image_height(image_height)
{
    position = glm::vec3(0, 0, -2);
    focal_center = position + glm::vec3(0, 0, focal_length);
    auto up = glm::vec3(0, 1, 0);
    vfov = vfov*PI/180; // degree to radian
    viewport_height = 2 * glm::tan(vfov/2) * focal_length; //applying vertical FOV
    viewport_width = viewport_height / aspect_ratio; // applying aspect ratio

    image_width = (int)image_height/ aspect_ratio;
    pixelWidth = viewport_width / image_width;
    pixelHeight = pixelWidth;

    pixelX = glm::normalize(glm::cross(focal_center - position, up)) * pixelWidth;
    pixelY = glm::normalize(glm::cross(pixelX, focal_center - position)) * pixelHeight;
    // std::cout << "img " << image_width << "x" << image_height << "\n";
    // std::cout << "vp  " << viewport_width << "x" << viewport_height << "\n";
    // std::cout << "px  " << pixelWidth << " " << pixelHeight << "\n";
} 

void Camera::set_pos(glm::vec3 pos)
{
    position = pos;
    focal_center = position + glm::vec3(0, 0, focal_length);
    auto up = glm::vec3(0, 1, 0);
    pixelX = glm::normalize(glm::cross(focal_center - position, up)) * pixelWidth;
    pixelY = glm::normalize(glm::cross(pixelX, focal_center - position)) * pixelHeight;
}

ray Camera::getRay(int width, int height)
{   
    using namespace glm;
    // float x = focal_center.x - viewport_width/2 + (width + 0.5)*pixelWidth;   
    // float y = focal_center.y - viewport_height/2 + (height + 0.5)*pixelHeight; 
    // vec3 dir = vec3(x, y, focal_center.z) - position;
    auto cornerPos = focal_center + (float)image_height/2 * pixelY - (float)image_width/2 * pixelX;
    auto pixelPos = cornerPos - (float)height * pixelY + (float)width * pixelX;
    return ray(position, pixelPos - position);
}



