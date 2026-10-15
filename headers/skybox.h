#pragma once
#include <string>
#include <glm/glm.hpp>
#include "texture.h"

class Skybox {
public:
    Skybox();
    Skybox(const char* path, float rotation_angle = 0.0f);
    glm::vec3 sampleColor(const glm::vec3& direction) const;

private:
    std::string file_path;
    float rotation_angle; // radians
    Texture texture;
};