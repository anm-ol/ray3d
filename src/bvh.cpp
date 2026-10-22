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
    if (!rootNode || rootNode->triangles.empty())
        return;

    // Calculate the bounding box
    Triangle *first = rootNode->triangles[0];

    rootNode->bbox.min = glm::min(
        glm::min(first->v0, first->v1), first->v2
    );

    rootNode->bbox.max = glm::max(
        glm::max(first->v0, first->v1), first->v2
    );

    for (Triangle *t : rootNode->triangles)
    {
        rootNode->bbox.min = glm::min(
            rootNode->bbox.min,
            glm::min(glm::min(t->v0, t->v1), t->v2)
        );

        rootNode->bbox.max = glm::max(
            rootNode->bbox.max,
            glm::max(glm::max(t->v0, t->v1), t->v2)
        );
    }

    // Stop at maximum depth or when splitting is unnecessary
    constexpr size_t MAX_LEAF_TRIANGLES = 4;

    if (depth <= 0 ||
        rootNode->triangles.size() <= MAX_LEAF_TRIANGLES)
        return;

    // Find the longest bounding-box axis
    glm::vec3 extent = rootNode->bbox.max - rootNode->bbox.min;

    int axis = 0;
    if (extent.y > extent.x)
        axis = 1;
    if (extent.z > extent[axis])
        axis = 2;

    // Sort triangles by centroid along the selected axis
    auto centroid = [axis](Triangle *t)
    {
        return (t->v0[axis] + t->v1[axis] + t->v2[axis]) / 3.0f;
    };

    std::sort(
        rootNode->triangles.begin(),
        rootNode->triangles.end(),
        [&](Triangle *a, Triangle *b)
        {
            return centroid(a) < centroid(b);
        }
    );

    // Split at the median
    size_t mid = rootNode->triangles.size() / 2;

    if (mid == 0 || mid == rootNode->triangles.size())
        return;

    rootNode->leftChild = new BVHNode();
    rootNode->rightChild = new BVHNode();

    rootNode->leftChild->triangles.assign(
        rootNode->triangles.begin(),
        rootNode->triangles.begin() + mid
    );

    rootNode->rightChild->triangles.assign(
        rootNode->triangles.begin() + mid,
        rootNode->triangles.end()
    );

    // Internal nodes no longer need their triangle list
    rootNode->triangles.clear();

    // Recursively build children
    splitBVH(rootNode->leftChild, depth - 1);
    splitBVH(rootNode->rightChild, depth - 1);
}

void BVH::deleteTree(BVHNode *node)
{
    if (!node) return;

    deleteTree(node->leftChild);
    deleteTree(node->rightChild);
    delete node;
}
