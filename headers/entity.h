#pragma once

#include <vector>
#include <string>
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

class Triangle : public Entity {
public:
    glm::vec3 v0, v1, v2;
    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
    bool ray_hit(ray& ray, hitInfo& hit) override;
    void scatter(hitInfo& hit, Material& mat) override;
};

class Mesh : public Entity {
public:
    std::vector<Triangle> triangles;
    Mesh(const std::string& mesh_file);
    Mesh(std::vector<Triangle> triangles): triangles(std::move(triangles)) {}
    bool ray_hit(ray& r, hitInfo& hit) override;
    void scatter(hitInfo& hit, Material& mat) override;
};