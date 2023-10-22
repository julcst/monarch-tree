#pragma once

#include "tree/generator.hpp"
#include "tree/model.hpp"
#include "framework/app.hpp"
#include "framework/camera.hpp"
#include "framework/gl/mesh.hpp"
#include "framework/gl/program.hpp"

class MainApp : public App {
public:
    MainApp();
protected:
    void init() override;
    void buildImGui() override;
    void render() override;
    void keyCallback(Key key, Action action) override;
    void scrollCallback(float amount) override;
    void moveCallback(const vec2& movement, bool leftButton, bool rightButton, bool middleButton) override;
private:
    Camera camera;
    Mesh fullscreenTriangle;
    Program treeShader;
    GLuint lAspectRatio;
    GLuint lFrames;
    GLuint lCameraPosition;
    GLuint lCameraRotation;
    GLuint lFocalLength;
    GLuint lTree;
    GLuint lNumBranches;
    GLuint lAABBCenter;
    GLuint lAABBSize;
    Tree::Generator treeGenerator;
    Tree::Model tree;
};