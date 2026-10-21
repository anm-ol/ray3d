#pragma once

#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "ray.h"
#include "material.h"

class BVH;

class Entity {
public:
    glm::vec3 center{0.0f};
    uint32_t materialID;
    
    virtual ~Entity() = default;
    virtual bool ray_hit(ray& r, hitInfo& hit) = 0;
};

class Sphere : public Entity {
public:
    float radius{1.0f};
    Sphere(glm::vec3 c, float r);
    bool ray_hit(ray& r, hitInfo& hit) override;
};

class Triangle : public Entity {
public:
    glm::vec3 v0, v1, v2;
    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
    bool ray_hit(ray& ray, hitInfo& hit) override;
};

class Mesh : public Entity {
public:
    ~Mesh() override;
    Mesh(std::vector<Triangle> triangles);
    Mesh(const std::string& mesh_file, glm::vec3 c = glm::vec3{1.0f}, glm::vec3 scale = glm::vec3{1.0f});
    bool ray_hit(ray& r, hitInfo& hit) override;

    void setCenter(const glm::vec3& c);
    void setScale(const glm::vec3& s);

private:
    std::vector<Triangle> triangles;
    std::unique_ptr<BVH> bvh;

    glm::vec3 scale {1.0f};
    bool useBVH = true;
    int bvhDepth = 64;

    glm::mat4 model;
    glm::mat4 invModel;

    void updateTransform();
};