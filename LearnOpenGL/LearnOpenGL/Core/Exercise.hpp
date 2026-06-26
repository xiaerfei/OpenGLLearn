//
//  Exercise.hpp
//  LearnOpenGL
//
//  一个"练习/场景"的抽象：拥有自己的场景相机，知道如何渲染自身。
//  双视角观察器的右侧用 sceneCamera 渲染它，左侧观察者会可视化这个相机的视锥与朝向。
//  写 learnopengl-cn 练习时，继承本类并实现 setup/update/render 即可。
//

#pragma once

#include "Camera.hpp"

class Exercise {
public:
    virtual ~Exercise() = default;

    // GL 上下文就绪后调用一次：创建着色器、网格、纹理等资源。
    virtual void setup() {}

    // 每帧逻辑更新。dt = 距上帧秒数，time = 自启动累计秒数。
    virtual void update(float dt, float time) {}

    // 用给定相机把场景渲染一遍。
    // 注意：同一帧会被调用两次 —— 一次用观察者相机（左视角），一次用 sceneCamera（右视角）。
    virtual void render(const Camera& camera) = 0;

    // 练习自己的相机：右侧"场景视角"使用，左侧"观察者视角"会画出它的视锥体。
    Camera sceneCamera;
};
