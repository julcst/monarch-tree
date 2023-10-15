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
    void init();
    void buildImGui();
    void render();
    void keyCallback(Key key, Action action);
    void scrollCallback(float amount);
    void moveCallback(vec2 movement, bool leftButton, bool rightButton, bool middleButton);
private:
    Camera camera;
    Mesh fullscreenTriangle;
    Program treeShader;
    GLuint lRes;
    GLuint lT;
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