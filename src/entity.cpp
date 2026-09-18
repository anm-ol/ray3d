#include "entity.h"
#include <iostream>

Sphere::Sphere(glm::vec3 c, float r): radius(r)
{
    center = c;
}

using namespace std;
float Sphere::ray_hit(ray& r)
{
    float a = glm::dot(r.dir, r.dir);
    float b = glm::dot(r.orig - center, r.dir);
    float c = glm::dot(r.orig - center, r.orig - center) - radius*radius;
    

    float d = (b*b - a*c);

    // cout << "b: " << r.orig << ", ac: " << center  << endl;
    // TODO
    return d;
}