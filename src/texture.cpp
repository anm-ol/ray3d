#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <algorithm>
#include <cmath>


Texture::Texture(const char* file){
    data =  stbi_loadf(file , &w , &h ,  &c , 3);
}
    Texture::~Texture(){
        if(data != nullptr){
            stbi_image_free(data);
        }
    }
    //sample function
    
    glm::vec3 Texture::sample(float u , float v, bool repeatU) const {
    if (data == nullptr)
    {
        return glm::vec3(0.0f , 0.0f , 0.0f);
    }
    u = repeatU ? u - std::floor(u) : std::clamp(u, 0.0f, 1.0f); // panorama wraps horizontally //horizontal image coordinate
    v = std::clamp(v, 0.0f, 1.0f); //vertical image coordinate 

    //Horizontal coordinates wrap: u = 1.2 becomes 0.2, and u = -0.2 becomes 0.8.

    float x = u * (repeatU ? w : w - 1); 
    float y = v * (h - 1);
    int left = x;
    int top = y;
    int right = repeatU ? (left + 1) % w : std::min(left + 1, w - 1); //makes the right neighbor wrap to column 0 when the left neighbor is at the panorama’s last column
    int bottom = std::min(top + 1, h - 1);

    glm::vec3 topColor = glm::mix(pixel(left, top), pixel(right, top), x - left);
    glm::vec3 bottomColor = glm::mix(pixel(left, bottom), pixel(right, bottom), x - left);
    return glm::mix(topColor, bottomColor, y - top);

}

glm::vec3 Texture::pixel(int column, int row) const {
    int index = (row * w + column) * 3;
    return glm::vec3(data[index], data[index + 1], data[index + 2]);
}
