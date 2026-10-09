#include <glm/glm.hpp>
#include <numbers>
#include "camera.h"
#include <iostream>

#define PI 3.14159

glm::vec3 eulerAngleToDir(float pitch, float yaw)
{
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    return direction;
}

Camera::Camera(float f, float ar, float vfov, int image_height) : focal_length(f), aspect_ratio(ar), image_height(image_height)
{
    position = glm::vec3(0, 0, -2);

    pitch = 0;
    yaw = 90;
    frontDir = eulerAngleToDir(pitch, yaw);
    upDir = eulerAngleToDir(pitch + 90, yaw);
    
    setOrientation();

    // focal_center = position + glm::vec3(0, 0, focal_length);
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

void Camera::setPosition(glm::vec3 pos)
{   
    auto offset = pos - position;
    position = pos;
    focal_center += offset;
    pixelX = glm::normalize(glm::cross(focal_center - position, upDir)) * pixelWidth;
    pixelY = glm::normalize(glm::cross(pixelX, focal_center - position)) * pixelHeight;
}

void Camera::setOrientation()
{
    focal_center = position + glm::normalize(frontDir) * focal_length;

    auto up = upDir;
    pixelX = glm::normalize(glm::cross(focal_center - position, up)) * pixelWidth;
    pixelY = glm::normalize(glm::cross(pixelX, focal_center - position)) * pixelHeight;    
}

void Camera::processCameraMovement(float xoffset, float yoffset)
{

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    std::cout << xoffset << " :  " << yoffset << std::endl;
    this->yaw   += xoffset;
    this->pitch += yoffset;
    std::cout << pitch << " :  " << yaw << std::endl;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    frontDir = eulerAngleToDir(pitch, yaw);
    upDir = eulerAngleToDir(pitch + 90, yaw);
    
    setOrientation();
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



