#include "ray.h"

ray::ray(glm::vec3 orig, glm::vec3 dir): orig(orig), dir(dir) {}

glm::vec3 ray::at(float t)
{ 
    return orig + dir*t;
}