#pragma once

#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "ray.h"
#include "material.h"

class BVH;

class Transform {
public:
    glm::vec3 pos;
    glm::vec3 rot;    // in degrees
    glm::vec3 scale;

    glm::mat4 model;
    glm::mat4 invModel;

    Transform(
        glm::vec3 pos = glm::vec3{0.0f}, 
        glm::vec3 rot = glm::vec3{0.0f}, 
        glm::vec3 scale = glm::vec3{1.0f}
    );

    // init/recompute model and invModel
    void update();
};

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
    glm::vec3 edge1, edge2;
    glm::vec3 faceNormal;

    glm::vec3 vn0, vn1, vn2;
    bool hasVertexNormal = false;

    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 vn0, glm::vec3 vn1, glm::vec3 vn2);
    bool ray_hit(ray& ray, hitInfo& hit) override;
};

class Mesh : public Entity {
public:
    Transform transform;

    ~Mesh() override;
    Mesh(std::vector<Triangle> triangles, Transform transform = Transform{});
    Mesh(const std::string& mesh_file, Transform transform = Transform{});

    bool ray_hit(ray& r, hitInfo& hit) override;

private:
    std::vector<Triangle> triangles;
    std::unique_ptr<BVH> bvh;

    bool useBVH = true;
    int bvhDepth = 64;
};