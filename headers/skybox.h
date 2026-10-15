#pragma once
#include <string>
#include <glm/glm.hpp>
#include "texture.h"

class Skybox {
public:
    Skybox(const glm::vec3& envLight = glm::vec3(0.0f));
    Skybox(const char* path, float rotation_angle = 0.0f);
    glm::vec3 sampleColor(const glm::vec3& direction) const;

private:
    std::string file_path;
    float rotation_angle; // radians
    Texture texture;
    glm::vec3 envLight = glm::vec3(0.0f);
};
