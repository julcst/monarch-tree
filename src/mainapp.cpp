#include "mainapp.hpp"

#include "framework/gl/program.hpp"
#include "framework/gl/mesh.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>

#include <vector>
#include <iostream>

using namespace glm;

MainApp::MainApp() : App(800, 600), camera(0.0f, 0.0f, 30.0f, 5.0f, 70.0f), treeGenerator(Tree::Config {507, 0.3f, 1.0f}) {
    fullscreenTriangle.load(FULLSCREEN_VERTICES, FULLSCREEN_INDICES);
    treeShader.load("screen.vert", "raycast.frag");
    lRes = treeShader.uniform("uRes");
    lT = treeShader.uniform("uT");
    lFrames = treeShader.uniform("uFrames");
    lCameraPosition = treeShader.uniform("uCameraPosition");
    lCameraRotation = treeShader.uniform("uCameraRotation");
    lFocalLength = treeShader.uniform("uFocalLength");
    lTree = treeShader.uniform("uBranches");
    lNumBranches = treeShader.uniform("uNumBranches");
    lAABBCenter = treeShader.uniform("uAABBCenter");
    lAABBSize = treeShader.uniform("uAABBSize");
}

void MainApp::init() {
    ImGui::GetStyle().ScaleAllSizes(1.2);
    ImGui::GetIO().FontGlobalScale = 1.2;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    tree = treeGenerator.generate();
    treeShader.set(lTree, tree.branches.data()->start, tree.branches.size() * 2);
    treeShader.set(lNumBranches, (unsigned int) tree.branches.size());
    treeShader.set(lAABBCenter, tree.aabb.center);
    treeShader.set(lAABBSize, tree.aabb.size);
}

void MainApp::render() {
    glClear(GL_DEPTH_BUFFER_BIT);
    // Render color pattern in the background
    glDepthMask(GL_FALSE);
    treeShader.bind();
    treeShader.set(lRes, resolution);
    treeShader.set(lT, time);
    treeShader.set(lFrames, frames);
    treeShader.set(lCameraPosition, camera.getPosition());
    treeShader.set(lCameraRotation, camera.calcRotation());
    treeShader.set(lFocalLength, 2.0f);
    fullscreenTriangle.draw();
}

void MainApp::keyCallback(Key key, Action action) {
    if(key == Key::ESC && action == Action::PRESS) close();
}

void MainApp::scrollCallback(float amount) {
    camera.zoom(amount);
}

void MainApp::moveCallback(vec2 movement, bool leftButton, bool rightButton, bool middleButton) {
    if(rightButton) camera.rotate(movement * 0.02f);
}

void MainApp::buildImGui() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
    ImGui::Begin("Statistics", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
    ImGui::Text("%2.1ffps avg: %2.1ffps %.0fx%.0f", 1.f / delta, ImGui::GetIO().Framerate, resolution.x, resolution.y);
    ImGui::PopStyleColor();
    ImGui::End();
}