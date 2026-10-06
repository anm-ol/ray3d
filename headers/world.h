#pragma once
#include <vector>
#include <memory>
#include "entity.h"
#include "camera.h"

class World
{
public:
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<Material> materials;
    vector<glm::vec4> pixels;
    vector<glm::vec4> accumPixels;
    int sampleCount;

    Camera cam;

    glm::vec3 envLight;
    
    World(); // default world
    glm::vec3 trace(ray& r, int max_bounces);
    bool ray_hit(ray& r, hitInfo& hit, uint32_t& entityID);
    void sampleRays(bool accumulate);

};

// should this class be renamed to world?