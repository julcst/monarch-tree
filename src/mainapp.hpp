#pragma once

#include "tree/generator.hpp"
#include "tree/model.hpp"
#include "framework/app.hpp"
#include "framework/camera.hpp"
#include "framework/mesh.hpp"
#include "framework/gl/program.hpp"
#include "framework/gl/buffer.hpp"

class MainApp : public App {
public:
    MainApp();
protected:
    void buildImGui() override;
    void render() override;
    void resizeCallback(const vec2& res) override;
    void keyCallback(Key key, Action action, Modifier modifier) override;
    void scrollCallback(float amount) override;
    void moveCallback(const vec2& movement, bool leftButton, bool rightButton, bool middleButton) override;
private:
    Camera camera;
    Mesh fullscreenTriangle;
    Program treeShader;
    Buffer<GL_UNIFORM_BUFFER> treeBuffer;
    Tree::Generator treeGenerator;
    Tree::Model tree;
};