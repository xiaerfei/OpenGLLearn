//
//  DualViewportRenderer.cpp
//  LearnOpenGL
//

#include "DualViewportRenderer.hpp"
#include "Exercise.hpp"

#include <OpenGL/gl3.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

void DualViewportRenderer::setup() {
    debug_.setup();
    ready_ = true;
}

void DualViewportRenderer::update(float dt) {
    observer_.update(dt);
}

void DualViewportRenderer::onDrag(float dxPixels, float dyPixels) {
    // 横向拖拽改 yaw，纵向拖拽改 pitch；系数调成手感自然
    observer_.orbit(-dxPixels * 0.3f, dyPixels * 0.3f);
}

void DualViewportRenderer::onScroll(float deltaY) {
    observer_.zoom(-deltaY * 0.002f * observer_.distance);
}

void DualViewportRenderer::onPinch(float magnification) {
    // magnification 为本次手势增量（捏合<0，张开>0）。按当前距离比例缩放，手感更自然。
    observer_.zoom(-magnification * observer_.distance);
}

void DualViewportRenderer::onKeyZoom(bool zoomIn) {
    float step = 0.01f * observer_.distance;
    observer_.zoomTarget(zoomIn ? -step : step);
}

void DualViewportRenderer::onKeyOrbit(bool right) {
    float step = 0.1f;
    observer_.orbitTarget(right ? step : -step, 0.0f);
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
