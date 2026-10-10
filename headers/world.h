#pragma once
#include <vector>
#include <memory>
#include "entity.h"
#include "camera.h"
#include "texture.h"

class World
{
public:
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<Material> materials;
    std::vector<glm::vec4> pixels;
    std::vector<glm::vec4> accumPixels;
    int sampleCount;

    Camera cam;
    Texture  skybox{"assets/noirlab2430b.hdr"};

    glm::vec3 envLight;
    
    World(); // default world
    glm::vec3 trace(ray& r, int max_bounces);
    bool ray_hit(ray& r, hitInfo& hit, uint32_t& entityID);
    void sampleRays(bool accumulate);

};

// should this class be renamed to world?
