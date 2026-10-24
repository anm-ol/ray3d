#include "entity.h"
#include "bvh.h"
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

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2): v0(v0), v1(v1), v2(v2) 
{
    edge1 = v1 - v0;
    edge2 = v2 - v0;
    faceNormal = glm::normalize(glm::cross(edge1, edge2));
    hasVertexNormal = false;
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 vn0, glm::vec3 vn1, glm::vec3 vn2):
    v0(v0), v1(v1), v2(v2), vn0(vn0), vn1(vn1), vn2(vn2)
{
    edge1 = v1 - v0;
    edge2 = v2 - v0;
    faceNormal = glm::normalize(glm::cross(edge1, edge2));
    hasVertexNormal = true;
}

bool Triangle::ray_hit(ray& r, hitInfo& hit)
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

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
    if(hasVertexNormal) // add option for switching back to flat shading
    {
        auto normal = (1-u-v)*vn0 + u*vn1 + v*vn2;
        hit.hitNormal = normal;
    }
    else
        hit.hitNormal = faceNormal;

    return true;
}

Mesh::~Mesh() = default;

Mesh::Mesh(std::vector<Triangle> triangles) : triangles(std::move(triangles))
{
    if (useBVH) { bvh = std::make_unique<BVH>(this->triangles, bvhDepth); }
    updateTransform();
}

Mesh::Mesh(const std::string& mesh_file, 
    glm::vec3 c, glm::vec3 scale, glm::vec3 rot) : scale(scale), rotation(rot)
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
        auto getVertex = [&](size_t index) {
            int vi = indices[index].vertex_index;

            return glm::vec3(
                attrib.vertices[3 * vi],
                attrib.vertices[3 * vi + 1],
                attrib.vertices[3 * vi + 2]
            );
        };
        auto getNormal = [&](size_t index) {
            int vn = indices[index].normal_index;

            return glm::vec3(
                attrib.normals[3 * vn],
                attrib.normals[3 * vn + 1],
                attrib.normals[3 * vn + 2]
            );
        };

        std::cout<< indices.size() << std::endl ;
        for (size_t i = 0; i+2 < indices.size(); i += 3) 
        {   
            if(indices[i].normal_index >= 0) // check if vertex normasl are available 
            {
                triangles.emplace_back(
                    getVertex(i),
                    getVertex(i+1),
                    getVertex(i+2),
                    getNormal(i),
                    getNormal(i+1),
                    getNormal(i+2)
                );
            }
            else
            {
                triangles.emplace_back(
                    getVertex(i),
                    getVertex(i+1),
                    getVertex(i+2)
                );
            }
        }
    }

    if (useBVH) { bvh = std::make_unique<BVH>(triangles, bvhDepth); }
    updateTransform();
}

void Mesh::updateTransform() 
{
    auto trans_mat = glm::translate(glm::mat4(1.0f), center);
    auto scale_mat = glm::scale(glm::mat4(1.0f), scale);

    auto rot_mat = glm::mat4(1.0f);
    rot_mat = glm::rotate(rot_mat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    rot_mat = glm::rotate(rot_mat, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    rot_mat = glm::rotate(rot_mat, glm::radians(rotation.z), glm::vec3(0, 0, 1));

    model = trans_mat * rot_mat * scale_mat;
    invModel = glm::inverse(model);
}

void Mesh::setRotation(const glm::vec3& rot) 
{
    rotation = rot;
    updateTransform();
}

void Mesh::setCenter(const glm::vec3& c) 
{
    center = c;
    updateTransform();
}

void Mesh::setScale(const glm::vec3& s) 
{
    scale = s;
    updateTransform();
}

bool Mesh::ray_hit(ray &r, hitInfo &hit)
{  
    // world space -> mesh space
    ray localRay;
    localRay.orig = glm::vec3(invModel * glm::vec4(r.orig, 1.0f));
    localRay.dir = glm::vec3(invModel * glm::vec4(r.dir, 0.0f));

    bool hitAnything = false;
    if (useBVH)
    {
        hitAnything = bvh->ray_hit(localRay, hit);
    }
    else 
    {
        for (auto& triangle : triangles) 
        {
            if (triangle.ray_hit(localRay, hit)) 
            {
                hitAnything = true;
            }
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