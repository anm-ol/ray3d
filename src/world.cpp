#include "world.h"
#include <random>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

using namespace std;

World::World()
{
// default world setup, should probably add stuff later to 
// actually init attributes of the world
    float ar = 9/16.0f;
    float f = 1.0f;
    cam = Camera(f, ar, 80, 512);
    envLight = glm::vec3(0.53, 0.81, 0.92)/10.0f; // sky background
    // envLight = glm::vec3(0);

    Material mat;
    mat = Material(glm::vec3(0.6, 0, 0), 0, 0.05f);
    materials.push_back(mat);
    mat = Material(glm::vec3(0.2, 0.8, 0), 1, 0.05f);
    materials.push_back(mat);
    mat = Material(glm::vec3(0.2, 0.2, 0.7), 1, 0.5f);
    materials.push_back(mat);
    mat = Material(glm::vec3(0, 0, 1), 1.0f); // emissive material
    materials.push_back(mat);

    std::shared_ptr<Sphere> s0 = std::make_shared<Sphere>(glm::vec3(2, 42, 2), 40.0f);
    s0->materialID = 0;
    entities.push_back(s0);
    
    std::shared_ptr<Sphere> s1 = std::make_shared<Sphere>(glm::vec3(0, 1, 2.5), 1.0f);
    s1->materialID = 1;
    entities.push_back(s1);

    std::shared_ptr<Sphere> s2 = std::make_shared<Sphere>(glm::vec3(2, -40, 2), 40.0f);
    s2->materialID = 2;
    entities.push_back(s2);

    std::shared_ptr<Sphere> s3 = std::make_shared<Sphere>(glm::vec3(1.5, 1, 1.0), 0.5f);
    s3->materialID = 3;
    entities.push_back(s3);

    std::shared_ptr<Mesh> mesh1 = std::make_shared<Mesh>("models/cube.obj", glm::vec3(0.0f, -0.25f, 1.0f), glm::vec3(0.25f, 0.5f, 0.25f));
    mesh1->materialID = 0;
    entities.push_back(mesh1);

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
            inLight += envLight * hit.rayColor;
            return inLight;
        }  
        auto materialID = entities[closestEntity]->materialID;
        auto material = materials[materialID];
        entities[closestEntity]->scatter(hit, material);

        inLight += material.emission() * hit.rayColor;
        hit.rayColor *= material.albedo;
        hit = hitInfo(hit.r_out, hit.rayColor);
    }
    return inLight;
}

void World::sampleRays(bool accumulate)
{      

    sampleCount++;

    int num_samples = 1;
    int max_bounces = 20;
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