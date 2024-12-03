#include "mainapp.hpp"

#include "framework/gl/program.hpp"
#include "framework/mesh.hpp"

#include "swarm/swarm.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <imgui.h>
#include <framework/imguiutil.hpp>

#include <vector>
#include <iostream>

using namespace glm;

constexpr size_t MAX_BRANCHES = 1024;
constexpr size_t MAX_BOIDS = 2048;

void uploadTreeData(const Tree::Model& tree, Buffer<GL_UNIFORM_BUFFER>& treeBuffer) {
    treeBuffer.set(tree.aabb.min, 0);
    treeBuffer.set(static_cast<uint>(tree.branches.size()), 3 * sizeof(float));
    treeBuffer.set(tree.aabb.max, sizeof(vec4));
    treeBuffer.set(tree.branches, 2 * sizeof(vec4));
}

MainApp::MainApp() : App(800, 600), treeGenerator(Tree::Config {507, 0.3f, 1.0f}) {
    fullscreenTriangle.load(Mesh::FULLSCREEN_VERTICES, Mesh::FULLSCREEN_INDICES);
    treeShader.load("shaders/raygen.vert", "shaders/raycast.frag");
    treeShader.bindUBO("Tree", 0);
    treeBuffer.allocate(MAX_BRANCHES * sizeof(vec4) * 2 + 2 * sizeof(vec4));
    treeBuffer.bind(0);

    std::printf("sizeof(Branch) = %lu\n", sizeof(Tree::Branch) / sizeof(float));

    const std::vector<float> boidVertices = {
        -0.01f, 0.0f, 0.0f,
        -0.1f, 0.0f,  0.1f,
        -0.1f, 0.0f, -0.1f,
         0.01f, 0.0f, 0.0f,
         0.1f, 0.0f, -0.1f,
         0.1f, 0.0f,  0.1f,
    };
    const std::vector<uint> boidIndices = {2, 1, 0, 5, 4, 3};
    boidMesh.load(boidVertices, boidIndices);
    boidShader.load("shaders/boid.vert", "shaders/boid.frag");
    boidShader.bindUBO("Swarm", 1);
    swarmBuffer.allocate(MAX_BOIDS * sizeof(Boid));
    swarmBuffer.bind(1);

    tree = treeGenerator.generate();
    camera.target.y = tree.aabb.min.y + 0.5f * (tree.aabb.max.y - tree.aabb.min.y);
    camera.worldPosition = vec3(20.0f, 0.0f, 0.0f);
    camera.invalidate();
    uploadTreeData(tree, treeBuffer);

    swarm.spawn(tree, true);

    glEnable(GL_DEPTH_TEST);
}

void MainApp::render() {
    glClear(GL_DEPTH_BUFFER_BIT);

    if (camera.updateIfChanged()) {
        treeShader.set("uClipToWorld", inverse(camera.projectionMatrix * camera.viewMatrix));
        treeShader.set("uCameraPosition", camera.worldPosition);
        treeShader.set("uWorldToClip", camera.projectionMatrix * camera.viewMatrix);
        boidShader.set("uWorldToClip", camera.projectionMatrix * camera.viewMatrix);
    }
    
    glDepthFunc(GL_ALWAYS);
    treeShader.use();
    fullscreenTriangle.draw();

    if (swarm.config.speed > 0.0f) {
        swarm.update(delta);
        swarmBuffer.set(swarm.boids, 0);
    }
    
    glDepthFunc(GL_LESS);
    boidShader.use();
    boidMesh.draw(swarm.config.nBoids);
}

void MainApp::resizeCallback(const vec2& res) {
    camera.resize(res.x / res.y);
    treeShader.set("uResolution", res);
}

void MainApp::keyCallback(Key key, Action action, Modifier modifier) {
    if (key == Key::ESC && action == Action::PRESS) close();
}

void MainApp::clickCallback(Button button, Action action, Modifier modifier) {
    if (button == Button::LEFT && action == Action::PRESS) {
        const vec4 cursorClip = vec4(convertCursorToClipSpace(), 1.0f, 1.0f);
        const vec4 cursorWorld = inverse(camera.projectionMatrix * camera.viewMatrix) * cursorClip;
        const vec3 origin = camera.worldPosition;
        const vec3 direction = normalize(vec3(cursorWorld) / cursorWorld.w - origin);
        swarm.flee(origin, direction);
    }
}

void MainApp::scrollCallback(float amount) {
    camera.zoom(amount);
}

void MainApp::moveCallback(const vec2& movement, bool leftButton, bool rightButton, bool middleButton) {
    if (rightButton || middleButton) camera.orbit(movement * 0.02f);
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);
    
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::SetNextWindowPos(ImVec2(0, 30));
    ImGui::SetNextWindowSizeConstraints(ImVec2(100, 0), ImVec2(300, 1000));
    ImGui::SetNextWindowBgAlpha(0.5f);
    ImGui::Begin("Config");

    if (ImGui::CollapsingHeader("Tree")) {
        bool changed = false;
        changed |= ImGui::SliderInt("Branches", reinterpret_cast<int*>(&treeGenerator.config.nBranches), 1, MAX_BRANCHES);
        changed |= ImGui::SliderFloat("Length", &treeGenerator.config.segmentLength, 0.1f, 1.0f);
        ImGui::SeparatorText("Factors");
        changed |= ImGui::SliderFloat("Forward", &treeGenerator.config.forwardFactor, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("Up", &treeGenerator.config.upFactor, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("Random", &treeGenerator.config.randomFactor, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("Spread", &treeGenerator.config.spreadFactor, 0.0f, 1.0f);
        ImGui::SeparatorText("Generation");
        ImGui::Checkbox("Fixed", &treeGenerator.config.fixedSeed);
        ImGui::SameLine();
        if (ImGui::Button("Shuffle")) {
            treeGenerator.shuffle();
            changed = true;
        }
        if (changed) {
            tree = treeGenerator.generate();
            camera.target.y = tree.aabb.min.y + 0.5f * (tree.aabb.max.y - tree.aabb.min.y);
            camera.invalidate();
            swarm.spawn(tree, true);
            uploadTreeData(tree, treeBuffer);
        }
    }

    if (ImGui::CollapsingHeader("Swarm")) {
        bool changed = false;
        changed |= ImGui::SliderInt("Boids", (int*)&swarm.config.nBoids, 1, MAX_BOIDS);
        ImGui::SeparatorText("View");
        changed |= ImGui::SliderFloat("Separation Range", &swarm.config.separationRange, 0.0f, swarm.config.viewRange);
        changed |= ImGui::SliderFloat("View Range", &swarm.config.viewRange, swarm.config.separationRange, 10.0f);
        changed |= ImGui::SliderAngle("View Angle", &swarm.config.viewAngle, 0.0f, 180.0f);
        changed |= ImGui::SliderInt("Neighbors", (int*)&swarm.config.neighborLimit, 1, 128);
        ImGui::SeparatorText("Forces");
        changed |= ImGui::SliderFloat("Speed", &swarm.config.speed, 0.0f, 10.0f);
        changed |= ImGui::SliderFloat("Separation", &swarm.config.separationFactor, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("Alignment", &swarm.config.alignmentFactor, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("Cohesion", &swarm.config.cohesionFactor, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("Random", &swarm.config.randomFactor, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("Forward", &swarm.config.forwardFactor, 0.0f, 2.0f);
        ImGui::SeparatorText("Interaction");
        changed |= ImGui::SliderFloat("Impact", &swarm.config.fleeFactor, -20.0f, 20.0f);
        ImGui::SeparatorText("Excitement");
        changed |= ImGui::SliderFloat("Max Move Radius", &swarm.config.maxMoveRadius, 0.0f, 100.0f);
        changed |= ImGui::SliderFloat("Max Move Time", &swarm.config.maxMoveTime, 0.0f, 20.0f);
        changed |= ImGui::SliderFloat("Variance", &swarm.config.excitementVariance, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("Mean", &swarm.config.excitementMean, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("Speed Boost", &swarm.config.excitementSpeedBoost, 0.0f, 10.0f);
        ImGui::SeparatorText("Homing");
        changed |= ImGui::SliderFloat("Speed", &swarm.config.homingSpeed, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("Pressure", &swarm.config.homingPressure, 0.0f, 1.0f);
        if (ImGui::Button("Reset")) {
            swarm.reset();
            changed = true;
        }
        if (changed) swarm.spawn(tree, false);
    }

    ImGui::End();
}