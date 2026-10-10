#pragma once
#include <glm/glm.hpp>
 

struct Texture {
    int w =0 , h =0 , c = 0;
    float* data = nullptr; 

    Texture() = default;
    Texture(const char* file);
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    ~Texture();

    glm::vec3 sample(float u , float v) const ;

private:
    glm::vec3 pixel(int column, int row) const;
};
