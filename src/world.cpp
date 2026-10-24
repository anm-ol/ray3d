#include "world.h"
#include <random>
#include <iostream>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

using namespace std;

World::World()
{
// default world setup, should probably add stuff later to 
// actually init attributes of the world

    float ar = 9/16.0f;
    float f = 1.0f;
    skybox.setTexture(glm::vec3(0.2f, 0.3f, 0.5f));
    cam = Camera(f, ar, 80, 512);

    Material mat;
    mat = Material(glm::vec3(0.6f, 0.0f, 0.0f), 0, 0.0f);
    materials.push_back(mat);
    mat = Material(glm::vec3(1.0f), 1, 0.0f);
    materials.push_back(mat); 
    mat = Material(glm::vec3(0.8f, 0.8f, 0.7f), 1, 0.05f);
    materials.push_back(mat);
    mat = Material(glm::vec3(0.2f, 0.2f, 0.7f), 1, 0.5f);
    materials.push_back(mat); 
    mat = Material(glm::vec3(1.0f), 1.0f);
    materials.push_back(mat); 
    mat = Material(glm::vec3(0.955f, 0.48f, 0.38f), 0.3f, 0.1f);
    materials.push_back(mat); 

    // Spheres
    auto s0 = std::make_shared<Sphere>(glm::vec3(2, 42, 2), 40.0f);
    s0->materialID = 0;
    // entities.push_back(s0);

    auto s1 = std::make_shared<Sphere>(glm::vec3(2, 1, -0.2f), 1.0f);
    s1->materialID = 5;
    entities.push_back(s1);

    auto s2 = std::make_shared<Sphere>(glm::vec3(2, -41, 2), 40.0f);
    s2->materialID = 2;
    entities.push_back(s2);

    auto s3 = std::make_shared<Sphere>(glm::vec3(-1.5f, 1, -1.0f), 0.5f);
    s3->materialID = 3;
    entities.push_back(s3);

    glm::vec3 center(0.0f, -1.0f, 1.0f);
    glm::vec3 scale(0.05f);
    glm::vec3 rotation(0.0f, 0.0f, 90.0f);
    auto mesh2 = std::make_shared<Mesh>("models/car.obj", center, scale, rotation);
    mesh2->materialID = 5;
    entities.push_back(mesh2);

    pixels = vector<glm::vec4>(cam.image_height*cam.image_width, glm::vec4(0.0,0,0,1));
    accumPixels = vector<glm::vec4>(cam.image_height*cam.image_width, glm::vec4(0.0,0,0,1));
    sampleCount = 0;
}

using namespace glm;

bool World::ray_hit(ray& r, hitInfo& hit, uint32_t& entityID)
{
    float last_t;
    vec3 albedo(1);
    for(size_t i=0; i<entities.size(); i++)
    {   
        last_t = hit.closest_t;
        auto e = entities[i];
        if(e->ray_hit(r, hit))
        {
            if(last_t != hit.closest_t)
            {    
                entityID = i;
                last_t = hit.closest_t;
            }
        }
    }
    if(hit.closest_t == INFINITY)
        return false;
    return true;
}

vec3 World::trace(ray& r, int max_bounces)
{
    hitInfo hit(r);
    vec3 inLight(0, 0, 0);

    for(int numBounce = 1; numBounce <= max_bounces; numBounce++)
    {       
        uint32_t closestEntity; 
        if(!ray_hit(hit.r_in, hit, closestEntity))
        {
            inLight += skybox.sampleColor(hit.r_in.dir) * hit.rayColor;
            return inLight;

        }  
        auto materialID = entities[closestEntity]->materialID;
        auto material = materials[materialID];
        material.scatter(hit);

        inLight += material.emission() * hit.rayColor;
        hit.rayColor *= material.albedo;
        hit = hitInfo(hit.r_out, hit.rayColor);
    }
    return inLight;
}

void World::sampleRays(bool accumulate)
{      

    sampleCount++;

    int num_samples = 3;
    int max_bounces = 5;
    for(int heightIndex=0; heightIndex<cam.image_height; heightIndex++)
    {
        for(int widthIndex=0; widthIndex<cam.image_width; widthIndex++)
        {
            int i = heightIndex * cam.image_width + widthIndex;
            ray r = cam.getRay(widthIndex, heightIndex);
            vec4 pixelColor = vec4(0);
            for(int j=0; j<num_samples; j++)
                pixelColor += vec4(trace(r, max_bounces), 1.0);
            pixelColor.w = 1.0;

            if (accumulate)
            {
                accumPixels[i] += pixelColor; 
                pixels[i] = accumPixels[i]/(float)sampleCount;
            }
            else
            {
                accumPixels[i] = vec4(0, 0, 0, 0);
                pixels[i] = pixelColor;
            }
        }
    }
    if (!accumulate)
        sampleCount = 0;
}
