#include "entity.h"
#include <iostream>
#include <glm/gtc/random.hpp>
Sphere::Sphere(glm::vec3 c, float r): radius(r)
{
    center = c;
    // emissiveColor = glm::vec3(0.7, 0.2, 0.4);
    // emissiveStrength = 0.6;
}


using namespace std;
bool Sphere::ray_hit(ray& r, hitInfo& hit)
{
    float a = glm::dot(r.dir, r.dir);
    float b = -glm::dot(r.orig - center, r.dir);
    float c = glm::dot(r.orig - center, r.orig - center) - radius*radius;
    

    float d = (b*b - a*c);
    if(d<0)
        return false;
    float t1 = ((b - glm::sqrt(d))/a);
    float t2 = ((b + glm::sqrt(d))/a);

    float t = t1 > 0 ? t1 : t2;
    if (t>0 && t < hit.closest_t)
    {
        hit.closest_t = t;

        hit.hitPoint = r.at(t);
        hit.hitNormal = (hit.hitPoint - center)/radius; 
        return true;
    }
    return false;
}

using namespace glm;
void Sphere::scatter(hitInfo& hit, Material& mat)
{
    vec3 orig;
    orig = hit.hitPoint + 0.01f*(hit.hitNormal);
    auto diffuseDir = glm::sphericalRand(1.0f) + hit.hitNormal;
    auto specularDir = glm::reflect(hit.r_in.dir, hit.hitNormal);

    auto outDir = glm::mix(specularDir, diffuseDir, mat.roughness); 
    hit.r_out = ray(orig, outDir);
}