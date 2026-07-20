//
//  OrbitCamera.hpp
//  LearnOpenGL
//
//  围绕目标点旋转的轨道相机，用于"观察者视角"：鼠标拖拽改变 yaw/pitch，滚轮改变距离。
//
//  输入只修改目标值（orbitTarget/zoomTargetScale），update() 每帧以帧率无关的
//  指数趋近插值到目标值，拖拽跟手、松手后自然滑行减速。
//

#pragma once

#include "Camera.hpp"

class OrbitCamera {
public:
    float yaw = -35.0f;    // 度，绕 Y 轴
    float pitch = 22.0f;   // 度，俯仰，clamp 到 (-89, 89)
    float distance = 6.0f; // 到目标的距离
    glm::vec3 target{0.0f};

    float fovYDegrees = 45.0f;
    float nearPlane = 0.05f;
    float farPlane = 200.0f;

    // 立即生效（同时同步目标值，避免被 update() 拉回）
    void orbit(float dYawDegrees, float dPitchDegrees);
    void zoom(float dDistance);

    // 平滑过渡：修改目标值，update() 每帧插值过去
    void orbitTarget(float dYawDegrees, float dPitchDegrees);
    void zoomTargetScale(float factor); // 距离按比例缩放（乘法），近慢远快
    void update(float dt);

    // 设置好 aspect 后产出可直接取 view/projection 的相机。
    Camera toCamera(float aspect) const;

private:
    glm::vec3 computePosition() const;

    float targetYaw_ = -35.0f;
    float targetPitch_ = 22.0f;
    float targetDistance_ = 6.0f;

    // 指数趋近速率（1/s）。时间常数 = 1/k：旋转 ~67ms 保证跟手；
    // 缩放 ~56ms——尾巴再长，近距离下会被察觉为松手后仍在"飘"。
    static constexpr float kOrbitSmoothSpeed = 15.0f;
    static constexpr float kZoomSmoothSpeed = 18.0f;
};
