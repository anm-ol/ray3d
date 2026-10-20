#include "skybox.h"
#include <cmath>
#define PI 3.14159f

Skybox::Skybox()
    : envLight(glm::vec3(0.53f, 0.81f, 0.92f) / 10.0f),
      rotation_angle(0.0f) {}

Skybox::Skybox(const char* path, float rotation_angle)
    : envLight(0.0f),
      file_path(path),
      rotation_angle(rotation_angle),
      texture(file_path.c_str()) {}

glm::vec3 Skybox::sampleColor(const glm::vec3& direction) const {
    if (texture.data == nullptr)
        return envLight;

    glm::vec3 d = glm::normalize(direction);

    float u = 0.5f + std::atan2(d.z, d.x) / (2.0f * PI);
    u += rotation_angle / (2.0f * PI);
    float v = 0.5f - std::asin(glm::clamp(d.y, -1.0f, 1.0f)) / PI;

    return texture.sample(u, v , true);
}
