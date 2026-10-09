#pragma once
#include <glm/glm.hpp>
#include <cmath>

class ray
{
public:
    glm::vec3 dir;
    glm::vec3 orig;
    
    ray() = default;
    ray(glm::vec3 orig, glm::vec3 dir);

    glm::vec3 at(float t);
};

struct hitInfo{
    ray r_in;
    ray r_out;
    
    glm::vec3 hitPoint;
    glm::vec3 hitNormal;
    glm::vec3 rayColor;

    float closest_t;
    int hitTriangleIdx = -1;

    hitInfo(ray r_in): r_in(r_in), closest_t(INFINITY), rayColor(glm::vec3(1.0f))
    {}
    hitInfo(ray r_in, glm::vec3 rayColor): r_in(r_in), closest_t(INFINITY), rayColor(rayColor)
    {}
};
