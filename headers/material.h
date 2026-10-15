#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

struct Material
{
    glm::vec3 albedo;
    glm::vec3 emissiveColor;
    float metallic;
    float roughness;
    float emissiveStrength;

    Material(): albedo(0), metallic(0), roughness(0), emissiveColor(0), emissiveStrength(0) {}
    Material(glm::vec3 albedo, float metal, float rough): albedo(albedo), metallic(metal), roughness(rough)
    {
        emissiveColor = glm::vec3(0);
        emissiveStrength = 0;
    } 
    Material(glm::vec3 emissiveColor, float emissiveStrength): emissiveColor(emissiveColor), emissiveStrength(emissiveStrength) 
    {
        albedo = glm::vec3(emissiveColor);
        metallic = 0;
        roughness = 0;
    }
    
    void scatter(hitInfo& hit)
    {
        glm::vec3 orig;
        orig = hit.hitPoint + 0.01f*(hit.hitNormal);
        auto diffuseDir = glm::sphericalRand(1.0f) + hit.hitNormal;
        auto specularDir = glm::reflect(hit.r_in.dir, hit.hitNormal);

        auto outDir = glm::mix(specularDir, diffuseDir, roughness); 
        hit.r_out = ray(orig, outDir);  
    }
    glm::vec3 emission() const{ return emissiveStrength*emissiveColor;}
};