//
//  DualViewportRenderer.cpp
//  LearnOpenGL
//

#include "DualViewportRenderer.hpp"
#include "Exercise.hpp"

#include <OpenGL/gl3.h>

#include <cmath>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

void DualViewportRenderer::setup() {
    debug_.setup();
    ready_ = true;
}

namespace {
constexpr float kTurnDegPerSec = 120.0f;  // A/D 转身角速度
constexpr float kMoveUnitsPerSec = 3.0f;  // W/S/Q/E 移动速度（网格一格 1 单位）
} // namespace

// 按键移动/转身不走目标值插值：每帧 dt 积分本身就是连续的，再套平滑
// 只会产生松手后的滑行尾巴（"飘"）。游戏手感 = 按下全速、松手即停。
void DualViewportRenderer::update(float dt) {
    if (dt > 0.0f) {
        if (inputTurn_ != 0.0f) {
            observer_.orbit(kTurnDegPerSec * inputTurn_ * dt, 0.0f);
        }

        // W/S/Q/E 在水平面移动目标点（相机跟随），方向取相机朝向的水平投影
        const float yawR = glm::radians(observer_.yaw);
        const glm::vec3 forward(-std::sin(yawR), 0.0f, -std::cos(yawR));
        const glm::vec3 right(std::cos(yawR), 0.0f, -std::sin(yawR));
        glm::vec3 moveDir = forward * inputForward_ + right * inputStrafe_;
        if (glm::dot(moveDir, moveDir) > 1.0f) {
            moveDir = glm::normalize(moveDir); // 斜向移动不加速
        }
        observer_.target += moveDir * kMoveUnitsPerSec * dt;
    }
    observer_.update(dt);
}

void DualViewportRenderer::setMovementInput(float strafe, float forward, float turn) {
    inputStrafe_ = strafe;
    inputForward_ = forward;
    inputTurn_ = turn;
}

// 所有输入只累积目标值，OrbitCamera::update() 每帧插值过去——拖拽跟手、松手后自然滑行

void DualViewportRenderer::onDrag(float dxPixels, float dyPixels) {
    // 横向拖拽改 yaw，纵向拖拽改 pitch；系数调成手感自然
    observer_.orbitTarget(-dxPixels * 0.3f, dyPixels * 0.3f);
}

void DualViewportRenderer::onScroll(float deltaY) {
    // 对数域等比缩放：每 100 单位 deltaY 改变约 18% 距离，近慢远快
    observer_.zoomTargetScale(std::exp(-deltaY * 0.002f));
}

void DualViewportRenderer::onPinch(float magnification) {
    // magnification 为本次手势增量（捏合<0，张开>0）
    observer_.zoomTargetScale(std::exp(-magnification));
}

void DualViewportRenderer::onKeyZoom(bool zoomIn) {
    observer_.zoomTargetScale(zoomIn ? 0.97f : 1.0f / 0.97f);
}

void DualViewportRenderer::onKeyOrbit(bool right) {
    observer_.orbitTarget(right ? 2.0f : -2.0f, 0.0f);
}

void DualViewportRenderer::render(int pixelW, int pixelH, Exercise& exercise) {
    if (!ready_ || pixelW <= 0 || pixelH <= 0) {
        return;
    }

    const int leftW = pixelW / 2;
    const int rightW = pixelW - leftW;
    const float leftAspect = (float)leftW / (float)pixelH;
    const float rightAspect = (float)rightW / (float)pixelH;

    // 场景相机按右侧宽高比设定，使观察者画出的视锥与右侧实际成像一致
    exercise.sceneCamera.aspect = rightAspect;

    glEnable(GL_SCISSOR_TEST);

    // ---- 左：观察者视角 ----
    glViewport(0, 0, leftW, pixelH);
    glScissor(0, 0, leftW, pixelH);
    glClearColor(0.06f, 0.07f, 0.09f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const Camera obs = observer_.toCamera(leftAspect);
    exercise.render(obs); // 场景本体（从外部看）

    debug_.begin();
    debug_.addGrid(5.0f, 1.0f, glm::vec3(0.25f, 0.27f, 0.30f));
    debug_.addAxes(glm::mat4(1.0f), 1.0f); // 世界坐标轴
    debug_.addFrustum(exercise.sceneCamera.view(), exercise.sceneCamera.projection(),
                      glm::vec3(0.95f, 0.85f, 0.2f)); // 场景相机视锥（黄）
    debug_.flush(obs.projection() * obs.view());

    // ---- 右：场景视角 ----
    glViewport(leftW, 0, rightW, pixelH);
    glScissor(leftW, 0, rightW, pixelH);
    glClearColor(0.10f, 0.12f, 0.14f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    exercise.render(exercise.sceneCamera);

    glDisable(GL_SCISSOR_TEST);
}

#pragma clang diagnostic pop
