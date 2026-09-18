#pragma once
#include <glm/glm.hpp>

class ray
{
public:
    glm::vec3 dir;
    glm::vec3 orig;
    
    ray(glm::vec3 orig, glm::vec3 dir);

    glm::vec3 at(float t);
};

