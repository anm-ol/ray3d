#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer.h"
#include "world.h"

// Adds the quad a,b,c,d as two triangles. List the corners counter-clockwise as
// seen from the side the surface should face: Triangle's normal is
// cross(b - a, c - a), and Material::scatter only bounces rays off that side.
static void addQuad(World& world, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, uint32_t materialID)
{
    auto t0 = std::make_shared<Triangle>(a, b, c);
    auto t1 = std::make_shared<Triangle>(a, c, d);
    t0->materialID = materialID;
    t1->materialID = materialID;
    world.entities.push_back(t0);
    world.entities.push_back(t1);
}

// Adds a closed box of the given full size, centred at `center`, rotated
// `yawDeg` about the Y axis. All faces point outward.
static void addBox(World& world, glm::vec3 center, glm::vec3 size, float yawDeg, uint32_t materialID)
{
    glm::mat4 xf = glm::translate(glm::mat4(1.0f), center) *
                   glm::rotate(glm::mat4(1.0f), glm::radians(yawDeg), glm::vec3(0, 1, 0));
    glm::vec3 h = size * 0.5f;
    auto P = [&](float sx, float sy, float sz) {
        return glm::vec3(xf * glm::vec4(sx * h.x, sy * h.y, sz * h.z, 1.0f));
    };

    // Corner numbering matches models/cube.obj
    glm::vec3 c1 = P(-1, -1, -1), c2 = P(1, -1, -1), c3 = P(1, 1, -1), c4 = P(-1, 1, -1);
    glm::vec3 c5 = P(-1, -1,  1), c6 = P(1, -1,  1), c7 = P(1, 1,  1), c8 = P(-1, 1,  1);

    addQuad(world, c1, c4, c3, c2, materialID); // -Z
    addQuad(world, c5, c6, c7, c8, materialID); // +Z
    addQuad(world, c1, c5, c8, c4, materialID); // -X
    addQuad(world, c2, c3, c7, c6, materialID); // +X
    addQuad(world, c1, c2, c6, c5, materialID); // -Y
    addQuad(world, c4, c8, c7, c3, materialID); // +Y
}

// Replaces the world's scene with a Cornell box.
//
// The room spans x,y in [-1, 1] and z in [0, 2]; its front face at z = 0 is
// open. The camera sits on the -Z side looking down +Z. With this camera's
// basis screen-left is +X, so the red wall is at x = +1 and the green wall at
// x = -1, matching the classic layout.
void setCornellBox(World& world)
{
    world.entities.clear();
    world.materials.clear();
    world.skybox.setTexture("assets/kloofendal_48d_partly_cloudy_puresky_4k.hdr"); // nothing outside the box emits light
    world.skybox.setTexture(glm::vec3(.0f));
    // Square image and a narrow FOV so the open front of the room fills the frame.
    world.cam = Camera(1.0f, 1.0f, 40.0f, 512);
    world.cam.setPosition(glm::vec3(0.0f, 0.0f, -3.7f));
    size_t numPixels = (size_t)world.cam.image_height * world.cam.image_width;
    world.pixels.assign(numPixels, glm::vec4(0, 0, 0, 1));
    world.accumPixels.assign(numPixels, glm::vec4(0, 0, 0, 1));
    world.sampleCount = 0;

    enum : uint32_t { WHITE = 0, RED, GREEN, LIGHT, SHINY };
    world.materials.push_back(Material(glm::vec3(0.73f), 0.0f, 1.0f));
    world.materials.push_back(Material(glm::vec3(0.65f, 0.05f, 0.05f), 0.0f, 1.0f));
    world.materials.push_back(Material(glm::vec3(0.12f, 0.45f, 0.15f), 0.0f, 1.0f));
    world.materials.push_back(Material(glm::vec3(1.0f, 0.9f, 0.75f), 15.0f));
    world.materials.push_back(Material(glm::vec3(0.7f), 0.95f, 0.01f)); // polished metal

    // Room walls, all facing inward.
    addQuad(world, {-1, -1, 0}, {-1, -1, 2}, { 1, -1, 2}, { 1, -1, 0}, WHITE); // floor,   normal +Y
    addQuad(world, {-1,  1, 0}, { 1,  1, 0}, { 1,  1, 2}, {-1,  1, 2}, WHITE); // ceiling, normal -Y
    addQuad(world, {-1, -1, 2}, {-1,  1, 2}, { 1,  1, 2}, { 1, -1, 2}, WHITE); // back,    normal -Z
    addQuad(world, { 1, -1, 0}, { 1, -1, 2}, { 1,  1, 2}, { 1,  1, 0}, RED);   // left,    normal -X
    addQuad(world, {-1, -1, 0}, {-1,  1, 0}, {-1,  1, 2}, {-1, -1, 2}, GREEN); // right,   normal +X

    // Area light just below the ceiling, facing down.
    const float ly = 0.98f, ls = 0.24f;
    addQuad(world, {-ls, ly, 1 - ls}, {ls, ly, 1 - ls}, {ls, ly, 1 + ls}, {-ls, ly, 1 + ls}, LIGHT);

    // Tall box at the back-left, short box at the front-right.
    // addBox(world, glm::vec3( 0.35f, -0.40f, 1.30f), glm::vec3(0.6f, 1.2f, 0.6f),  18.0f, WHITE);
    // addBox(world, glm::vec3(-0.35f, -0.70f, 0.65f), glm::vec3(0.6f, 0.6f, 0.6f), -15.0f, WHITE);'

    glm::vec3 center{0.0f, -0.3f, 1.0f};
    glm::vec3 scale{0.05f};
    std::shared_ptr<Mesh> mesh1 = std::make_shared<Mesh>("models/dragon.obj", center, scale);
    mesh1->materialID = WHITE;
    world.entities.push_back(mesh1);
}

int main()
{
    World world = World();
    setCornellBox(world);

    renderer scene(world, 800, 600);
    scene.render(world);

    return 0;
}
