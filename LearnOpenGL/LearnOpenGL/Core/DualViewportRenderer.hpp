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
    void update(float dt); // 每帧调用，驱动平滑插值

    // pixelW/pixelH：整个视图的像素尺寸。内部分割左右并各渲染一次 exercise。
    void render(int pixelW, int pixelH, Exercise& exercise);

    // 输入转发（来自 GLView 主线程）
    void onDrag(float dxPixels, float dyPixels);
    void onScroll(float deltaY);            // 鼠标滚轮 / 双指滑动
    void onPinch(float magnification);      // 触控板双指捏合
    void onKeyZoom(bool zoomIn);            // 上下箭头：精细缩放
    void onKeyOrbit(bool right);            // 左右箭头：精细旋转

    // 游戏式移动（魔兽世界布局）：W/S 前后、Q/E 平移、A/D 转身。
    // 主线程把按键状态合成每轴 -1/0/+1 传入，update() 按 dt 持续推进。
    void setMovementInput(float strafe, float forward, float turn);

    OrbitCamera& observer() { return observer_; }

private:
    OrbitCamera observer_;
    DebugDraw debug_;
    bool ready_ = false;

    // 移动输入（主线程写，渲染线程读）
    float inputStrafe_ = 0.0f;   // +右 -左
    float inputForward_ = 0.0f;  // +前 -后
    float inputTurn_ = 0.0f;     // +左转 -右转（yaw 增加 = 视野向左扫）
};
