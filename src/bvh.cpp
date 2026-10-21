#include "bvh.h"
#include "entity.h"

bool AABB::ray_hit(ray &ray, hitInfo &hit)
{
    // prevent division by zero
    float invDirX = ray.dir.x == 0 ? INFINITY : 1 / ray.dir.x;
    float invDirY = ray.dir.y == 0 ? INFINITY : 1 / ray.dir.y;
    float invDirZ = ray.dir.z == 0 ? INFINITY : 1 / ray.dir.z;

    // tmax -> max of all entry 
    // tmin -> min of all exit

    float tx1 = (min.x - ray.orig.x) * invDirX;
    float tx2 = (max.x - ray.orig.x) * invDirX;
    float tmin = glm::min(tx1, tx2);
    float tmax = glm::max(tx1, tx2);
    
    float ty1 = (min.y - ray.orig.y) * invDirY;
    float ty2 = (max.y - ray.orig.y) * invDirY;
    tmin = glm::max(tmin, glm::min(ty1, ty2));
    tmax = glm::min(tmax, glm::max(ty1, ty2));

    float tz1 = (min.z - ray.orig.z) * invDirZ;
    float tz2 = (max.z - ray.orig.z) * invDirZ;
    tmin = glm::max(tmin, glm::min(tz1, tz2));
    tmax = glm::min(tmax, glm::max(tz1, tz2));

    bool hitAnything = (tmin <= tmax) && (tmax > 0);
    if (hitAnything)
    {
        hit.closest_t = glm::max(tmin, 0.0f);
    }

    return hitAnything;
}


bool BVH::ray_hit(ray &r, hitInfo &hit)
{
    // start traversal only from rootNode
    if (!rootNode) return false;

    bool hitAnything = false;
    std::stack<BVHNode*> stack;
    stack.push(rootNode);

    while (!stack.empty())
    {
        BVHNode* node = stack.top();
        stack.pop();

        // ray doesn't intersect bbox; skip this node
        hitInfo boxHit(r);
        if (!node->bbox.ray_hit(r, boxHit)) continue;

        // skip nodes whose bbox is farther than the closest_t found so far
        if (boxHit.closest_t > hit.closest_t) continue;

        // leaf node; check triangles
        if (node->isLeafNode()) 
        {
            for (Triangle* t : node->triangles)
            {
                if (t->ray_hit(r, hit))
                {
                    hitAnything = true;
                }
            }

        } 

        // recurse child nodes
        else 
        {
            if (node->leftChild) stack.push(node->leftChild);
            if (node->rightChild) stack.push(node->rightChild);
        }
    }

    return hitAnything;
}

BVH::BVH(std::vector<Triangle> &triangles, int depth)
{
    rootNode = new BVHNode();

    rootNode->triangles.reserve(triangles.size());
    for (Triangle &triangle : triangles)
    {
        rootNode->triangles.push_back(&triangle);
    }

    // recursively split traingles: rootNode -> children
    splitBVH(rootNode, depth);
}

BVH::~BVH()
{
    deleteTree(rootNode);
}

// split to for bvh recursively
void BVH::splitBVH(BVHNode *rootNode, int depth)
{
    // TODO
}

void BVH::deleteTree(BVHNode *node)
{
    if (!node) return;

    deleteTree(node->leftChild);
    deleteTree(node->rightChild);
    delete node;
}
