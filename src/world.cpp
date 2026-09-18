#include "world.h"
#include <random>
#include <iostream>

World::World()
{
// default world setup, should probably add stuff later to 
// actually init attributes of the world
    float ar = 9/16.0f;
    float f = 1.0f;
    cam = Camera(f, ar, 90, 512);
    std::shared_ptr<Sphere> s = std::make_shared<Sphere>(glm::vec3(0, 0, 1), 0.5f);
    entities.push_back(s);
    std::cout << "camera:" <<cam.image_height*cam.image_width << std::endl;
    
    pixels = vector<glm::vec4>(cam.image_height*cam.image_width, glm::vec4(0.5,0,0,1));
}

float World::ray_hit(ray& r)
{
    // float t_min = 100000.0f;
    float t;
    for(std::shared_ptr<Entity> e: entities)
    {
        t = e->ray_hit(r);
    }
    return t;
}

using namespace std;
void World::sampleRays()
{   
    for(int heightIndex=0; heightIndex<cam.image_height; heightIndex++)
    {
        for(int widthIndex=0; widthIndex<cam.image_width; widthIndex++)
        {
            int i = heightIndex * cam.image_width + widthIndex;
            ray r = cam.getRay(widthIndex, heightIndex);
            float d = ray_hit(r);
            if(d >= 0)
                pixels[i] = glm::vec4(1,1,1,1);
            // std::cout<< d << std::endl;
        }
    }
} 