#pragma once
#include <vector>
#include <memory>
#include "entity.h"
#include "camera.h"

class World
{
public:
    std::vector<std::shared_ptr<Entity>> entities;
    vector<glm::vec4> pixels;
    Camera cam;

    World(); // default world
    float ray_hit(ray& r);
    void sampleRays();

};

// should this class be renamed to world?