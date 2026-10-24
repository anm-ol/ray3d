#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
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
    antialiasing = true;

    setOrientation();

    vfov = vfov*PI/180; // degree to radian
    viewport_height = 2 * glm::tan(vfov/2) * focal_length; //applying vertical FOV
    viewport_width = viewport_height / aspect_ratio; // applying aspect ratio

    image_width = (int)image_height/ aspect_ratio;
    pixelWidth = viewport_width / image_width;
    pixelHeight = pixelWidth;

    pixelX = glm::normalize(glm::cross(focal_center - position, upDir)) * pixelWidth;
    pixelY = glm::normalize(glm::cross(pixelX, focal_center - position)) * pixelHeight;
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

    this->yaw   += xoffset;
    this->pitch += yoffset;

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

    float xoffset = glm::linearRand(-1.0f, 1.0f)/2;
    float yoffset = glm::linearRand(-1.0f, 1.0f)/2;
    
    if (antialiasing)
    {
        float h = (float)height + yoffset;
        float w = (float)width + xoffset; // adding random jitter for anti-aliasing
    }
    else
    {
        float h = (float)height;
        float w = (float)width; // adding random jitter for anti-aliasing
    }
     
    auto cornerPos = focal_center + (float)image_height/2 * pixelY - (float)image_width/2 * pixelX;
    auto pixelPos = cornerPos - h * pixelY + w * pixelX;
    return ray(position, pixelPos - position);
}



