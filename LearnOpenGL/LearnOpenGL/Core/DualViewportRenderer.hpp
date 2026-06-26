//
//  DualViewportRenderer.hpp
//  LearnOpenGL
//
//  双视角编排器：把整个 GLView 左右切成两块视口。
//    左 = 观察者视角（轨道相机）：画世界网格 + 坐标轴 + 练习相机的视锥体，再叠加场景本体
//    右 = 场景视角：用练习自己的相机渲染
//  这是 Nate Robins "world-space view / screen-space view" 双窗口的现代单上下文实现。
//

#pragma once

#include "OrbitCamera.hpp"
#include "DebugDraw.hpp"

class Exercise;

class DualViewportRenderer {
public:
    void setup(); // 上下文就绪后调用

    // pixelW/pixelH：整个视图的像素尺寸。内部分割左右并各渲染一次 exercise。
    void render(int pixelW, int pixelH, Exercise& exercise);

    // 输入转发（来自 GLView 主线程）
    void onDrag(float dxPixels, float dyPixels);
    void onScroll(float deltaY);            // 鼠标滚轮 / 双指滑动
    void onPinch(float magnification);      // 触控板双指捏合

    OrbitCamera& observer() { return observer_; }

private:
    OrbitCamera observer_;
    DebugDraw debug_;
    bool ready_ = false;
};
