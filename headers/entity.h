#pragma once

#include <glm/glm.hpp>
#include "ray.h"
#include "material.h"

class Entity {
public:
    glm::vec3 center{0.0f};
    uint32_t materialID;

    virtual ~Entity() = default;
    virtual bool ray_hit(ray& r, hitInfo& hit) = 0;
    virtual void scatter(hitInfo& hit, Material& mat) = 0;
};

class Sphere : public Entity {
public:
    float radius{1.0f};
    Sphere(glm::vec3 c, float r);
    bool ray_hit(ray& r, hitInfo& hit) override;
    void scatter(hitInfo& hit, Material& mat) override;
};