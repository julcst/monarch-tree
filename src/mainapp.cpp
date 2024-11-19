#include "mainapp.hpp"

#include "framework/gl/program.hpp"
#include "framework/mesh.hpp"

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
    treeBuffer.allocate(1024 * sizeof(vec4));
    treeBuffer.bind(0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    tree = treeGenerator.generate();
    camera.target.y = tree.aabb.min.y + 0.5f * (tree.aabb.max.y - tree.aabb.min.y);
    camera.invalidate();
    uploadTreeData(tree, treeBuffer);
}

void MainApp::render() {
    glClear(GL_DEPTH_BUFFER_BIT);

    if (camera.updateIfChanged()) {
        treeShader.set("uCameraMatrix", camera.cameraMatrix);
        treeShader.set("uCameraPosition", camera.worldPosition);
        treeShader.set("uAspectRatio", camera.aspectRatio);
        treeShader.set("uFocalLength", camera.focalLength);
    }
    
    treeShader.use();
    fullscreenTriangle.draw();
}

void MainApp::resizeCallback(const vec2& res) {
    camera.resize(res.x / res.y);
    treeShader.set("uResolution", res);
}

void MainApp::keyCallback(Key key, Action action, Modifier modifier) {
    if (key == Key::ESC && action == Action::PRESS) close();
}

void MainApp::scrollCallback(float amount) {
    camera.zoom(amount);
}

void MainApp::moveCallback(const vec2& movement, bool leftButton, bool rightButton, bool middleButton) {
    if (leftButton || rightButton || middleButton) camera.orbit(movement * 0.02f);
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);
    
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::Begin("Tree config");
    bool changed = false;
    changed |= ImGui::SliderInt("n", reinterpret_cast<int*>(&treeGenerator.config.nBranches), 1, 507);
    changed |= ImGui::SliderFloat("length", &treeGenerator.config.segmentLength, 0.1f, 1.0f);
    changed |= ImGui::SliderFloat("forward", &treeGenerator.config.forwardFactor, 0.0f, 1.0f);
    changed |= ImGui::SliderFloat("up", &treeGenerator.config.upFactor, 0.0f, 1.0f);
    changed |= ImGui::SliderFloat("random", &treeGenerator.config.randomFactor, 0.0f, 1.0f);
    changed |= ImGui::SliderFloat("spread", &treeGenerator.config.spreadFactor, 0.0f, 1.0f);
    ImGui::Checkbox("fixed", &treeGenerator.config.fixedSeed);
    if (changed) {
        tree = treeGenerator.generate();
        camera.target.y = tree.aabb.min.y + 0.5f * (tree.aabb.max.y - tree.aabb.min.y);
        camera.invalidate();
        uploadTreeData(tree, treeBuffer);
    }
    ImGui::End();
}