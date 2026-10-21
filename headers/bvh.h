#pragma once

#include <algorithm>
#include <vector>
#include <stack>
#include <limits>
#include "ray.h"

class Triangle;

struct AABB {
public:
    glm::vec3 min, max;
    bool ray_hit(ray& r, hitInfo& hit);
};

struct BVHNode 
{
    AABB bbox;
    BVHNode* leftChild = nullptr;
    BVHNode* rightChild = nullptr;
    std::vector<Triangle*> triangles;
    
    // checking for leaf nodes
    bool isLeafNode() const 
    {
        return (leftChild == nullptr && rightChild == nullptr) &&
                triangles.size() > 0;
    }
};

class BVH 
{
public:
    BVH(std::vector<Triangle>& triangles, int depth);
    bool ray_hit(ray& r, hitInfo& hit);

    ~BVH();
    BVH(const BVH&) = delete;
    BVH& operator=(const BVH&) = delete;
    BVH(BVH&&) = delete;
    BVH& operator=(BVH&&) = delete;

private:
    BVHNode* rootNode = nullptr;
    void splitBVH(BVHNode* rootNode, int depth = 0);
    void deleteTree(BVHNode* node);
};
