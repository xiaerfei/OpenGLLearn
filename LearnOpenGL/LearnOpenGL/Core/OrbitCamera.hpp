//
//  OrbitCamera.hpp
//  LearnOpenGL
//
//  围绕目标点旋转的轨道相机，用于"观察者视角"：鼠标拖拽改变 yaw/pitch，滚轮改变距离。
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

    void orbit(float dYawDegrees, float dPitchDegrees);
    void zoom(float dDistance);

    // 设置好 aspect 后产出可直接取 view/projection 的相机。
    Camera toCamera(float aspect) const;

private:
    glm::vec3 computePosition() const;
};
