#include "entity.h"
#include <iostream>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Sphere::Sphere(glm::vec3 c, float r): radius(r)
{
    center = c;
    // emissiveColor = glm::vec3(0.7, 0.2, 0.4);
    // emissiveStrength = 0.6;
}


using namespace std;
bool Sphere::ray_hit(ray& r, hitInfo& hit)
{
    float a = glm::dot(r.dir, r.dir);
    float b = -glm::dot(r.orig - center, r.dir);
    float c = glm::dot(r.orig - center, r.orig - center) - radius*radius;

    float d = (b*b - a*c);
    if(d<0)
        return false;
    float t1 = ((b - glm::sqrt(d))/a);
    float t2 = ((b + glm::sqrt(d))/a);

    float t = t1 > 0 ? t1 : t2;
    if (t>0 && t < hit.closest_t)
    {
        hit.closest_t = t;

        hit.hitPoint = r.at(t);
        hit.hitNormal = (hit.hitPoint - center)/radius; 
        return true;
    }
    return false;
}

using namespace glm;
void Sphere::scatter(hitInfo& hit, Material& mat)
{
    vec3 orig;
    orig = hit.hitPoint + 0.01f*(hit.hitNormal);
    auto diffuseDir = glm::sphericalRand(1.0f) + hit.hitNormal;
    auto specularDir = glm::reflect(hit.r_in.dir, hit.hitNormal);

    auto outDir = glm::mix(specularDir, diffuseDir, mat.roughness); 
    hit.r_out = ray(orig, outDir);
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2): v0(v0), v1(v1), v2(v2) {}

bool Triangle::ray_hit(ray& r, hitInfo& hit)
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;      

    glm::vec3 h = glm::cross(r.dir, edge2);
    float det = glm::dot(edge1, h);

    // Ray is parallel to triangle
    if (abs(det) < epsilon) 
        return false; 
 
    float f = 1.0f / det;

    glm::vec3 s = r.orig - v0;
    float u = f * glm::dot(s, h);

    // Ray passes outside edge2's bounds
    if (u < -epsilon || u - 1 > epsilon)
        return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(r.dir, q);

    // Ray passes outside edge1's bounds
    if (v < -epsilon || u + v - 1 > epsilon)
        return false;  

    // The ray line intersects with the triangle.
    // We compute t to find where on the ray the intersection is.
    float t = f * glm::dot(edge2, q);

    // Reject intersections behind the ray or
    // farther than the closest existing hit.
    if (t <= epsilon || t >= hit.closest_t)
        return false;

    hit.closest_t = t;
    hit.hitPoint = r.at(t);
    hit.hitNormal = glm::normalize(glm::cross(edge1, edge2));

    return true;
}

void Triangle::scatter(hitInfo& hit, Material& mat)
{
    vec3 orig;
    orig = hit.hitPoint + 0.01f*(hit.hitNormal);
    auto diffuseDir = glm::sphericalRand(1.0f) + hit.hitNormal;
    auto specularDir = glm::reflect(hit.r_in.dir, hit.hitNormal);

    auto outDir = glm::mix(specularDir, diffuseDir, mat.roughness); 
    hit.r_out = ray(orig, outDir);
}

Mesh::Mesh(const std::string& mesh_file, 
    glm::vec3 c, glm::vec3 scale) : scale(scale)
{
    center = c;

    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config;
    config.triangulate = true;

    if(!reader.ParseFromFile(mesh_file, config)) 
    {
        throw std::runtime_error(reader.Error());
    }

    const auto& attrib = reader.GetAttrib();    // vertex positions 
    const auto& shapes = reader.GetShapes();    // vertex indices 

    for (const auto& shape : shapes) 
    {
        const auto& indices = shape.mesh.indices;
        for (size_t i = 0; i+2 < indices.size(); i += 3) 
        {   
            auto getVertex = [&](size_t index) {
                int vi = indices[index].vertex_index;

                return glm::vec3(
                    attrib.vertices[3 * vi],
                    attrib.vertices[3 * vi + 1],
                    attrib.vertices[3 * vi + 2]
                );
            };

            triangles.emplace_back(
                getVertex(i),
                getVertex(i+1),
                getVertex(i+2)
            );
        }
    }

    updateTransform();
}

void Mesh::updateTransform() 
{
    model = glm::translate(glm::mat4(1.0f), center) * 
            glm::scale(glm::mat4(1.0f), scale);
    invModel = glm::inverse(model);
}

void Mesh::setCenter(const glm::vec3& c) {
    center = c;
    updateTransform();
}

void Mesh::setScale(const glm::vec3& s) {
    scale = s;
    updateTransform();
}

bool Mesh::ray_hit(ray &r, hitInfo &hit)
{  
    // world space -> mesh space
    ray localRay;
    localRay.orig = glm::vec3(
        invModel * glm::vec4(r.orig, 1.0f)
    );
    localRay.dir = glm::vec3(
        invModel * glm::vec4(r.dir, 0.0f)
    );

    bool hitAnything = false;

    for (size_t i = 0; i < triangles.size(); i++) 
    {
        if (triangles[i].ray_hit(localRay, hit)) 
        {
            hitAnything = true;
            hit.hitTriangleIdx = i;
        }
    }

    if (hitAnything)
    {
        // convert mesh space -> world space
        hit.hitPoint = glm::vec3(model * glm::vec4(hit.hitPoint, 1.0f));
        hit.hitNormal = glm::normalize(glm::transpose(glm::mat3(invModel)) * hit.hitNormal);
    }

    return hitAnything;
}

void Mesh::scatter(hitInfo& hit, Material& mat)
{
    if (hit.hitTriangleIdx < 0 || hit.hitTriangleIdx >= triangles.size()) return;

    triangles[hit.hitTriangleIdx].scatter(hit, mat);    
}