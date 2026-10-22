#pragma once
#include <glm/glm.hpp>
#include "texture.h"

class Skybox {
public:
    Skybox();
    Skybox(const char* path, float rotation_angle);
    bool setTexture(const char* path, float rotation_angle = 0.0f);
    void setTexture(const glm::vec3& color);
    glm::vec3 sampleColor(const glm::vec3& direction) const;
    glm::vec3 envLight;

private:
    float rotation_angle; // radians
    Texture texture;
};
