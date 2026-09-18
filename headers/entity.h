#pragma once

#include <glm/glm.hpp>
#include "ray.h"

class Entity
{
public:
    glm::vec3 center;

    virtual float ray_hit(ray& r)
    {
        return 5.0f;
    }

};

class Sphere : public Entity
{
public:
    float radius;
    glm::vec3 color;

    Sphere(glm::vec3 c, float r);

    float ray_hit(ray& r);

};