#pragma once
#include <vector>
#include <memory>
#include <string>
#include "entity.h"
#include "camera.h"
#include "skybox.h"

class World
{
public:
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<Material> materials;
    std::vector<glm::vec4> pixels;
    std::vector<glm::vec4> accumPixels;
    int sampleCount;

    Camera cam;
    std::unique_ptr<Skybox> skybox;

    glm::vec3 envLight;
    
    World(const std::string& skyboxPath = "");
    glm::vec3 trace(ray& r, int max_bounces);
    bool ray_hit(ray& r, hitInfo& hit, uint32_t& entityID);
    void sampleRays(bool accumulate);

};

// should this class be renamed to world?
