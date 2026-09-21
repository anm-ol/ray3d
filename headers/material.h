#pragma once

#include <glm/glm.hpp>

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

    glm::vec3 emission() const{ return emissiveStrength*emissiveColor;}
};