//
//  OrbitCamera.cpp
//  LearnOpenGL
//

#include "OrbitCamera.hpp"

#include <algorithm>
#include <cmath>

namespace {

constexpr float kPitchLimit = 89.0f;
constexpr float kMinDistance = 0.5f;
constexpr float kMaxDistance = 100.0f;

// 帧率无关的指数趋近系数：dt 波动时收敛速度恒定，dt 很大时直接到位。
float smoothAlpha(float speed, float dt) {
    return 1.0f - std::exp(-speed * dt);
}

} // namespace

void OrbitCamera::orbit(float dYawDegrees, float dPitchDegrees) {
    yaw += dYawDegrees;
    pitch = std::clamp(pitch + dPitchDegrees, -kPitchLimit, kPitchLimit);
    targetYaw_ = yaw;
    targetPitch_ = pitch;
}

void OrbitCamera::zoom(float dDistance) {
    distance = std::clamp(distance + dDistance, kMinDistance, kMaxDistance);
    targetDistance_ = distance;
}

void OrbitCamera::orbitTarget(float dYawDegrees, float dPitchDegrees) {
    targetYaw_ += dYawDegrees;
    targetPitch_ = std::clamp(targetPitch_ + dPitchDegrees, -kPitchLimit, kPitchLimit);
}

void OrbitCamera::zoomTargetScale(float factor) {
    targetDistance_ = std::clamp(targetDistance_ * factor, kMinDistance, kMaxDistance);
}

void OrbitCamera::update(float dt) {
    if (dt <= 0.0f) {
        return;
    }

    // 残差低于亚像素量级就直接吸附停住：收敛尾巴的缓慢蠕动近距离下会被察觉为"飘"
    const float orbitA = smoothAlpha(kOrbitSmoothSpeed, dt);
    yaw += (targetYaw_ - yaw) * orbitA;
    pitch += (targetPitch_ - pitch) * orbitA;
    if (std::abs(targetYaw_ - yaw) < 2e-3f) yaw = targetYaw_;
    if (std::abs(targetPitch_ - pitch) < 2e-3f) pitch = targetPitch_;

    // 距离在对数空间插值：缩放全程等比推进，近处不会"急刹"。
    // 吸附阈值同样在对数域（相对 0.05%），近处远处的感知一致。
    const float logDiff = std::log(targetDistance_ / distance);
    if (std::abs(logDiff) < 5e-4f) {
        distance = targetDistance_;
    } else {
        distance *= std::exp(logDiff * smoothAlpha(kZoomSmoothSpeed, dt));
    }
}

glm::vec3 OrbitCamera::computePosition() const {
    const float yawR = glm::radians(yaw);
    const float pitchR = glm::radians(pitch);
    return target + glm::vec3(distance * std::cos(pitchR) * std::sin(yawR),
                              distance * std::sin(pitchR),
                              distance * std::cos(pitchR) * std::cos(yawR));
}

Camera OrbitCamera::toCamera(float aspect) const {
    Camera c;
    c.position = computePosition();
    c.target = target;
    c.up = glm::vec3(0.0f, 1.0f, 0.0f);
    c.fovYDegrees = fovYDegrees;
    c.aspect = aspect;
    c.nearPlane = nearPlane;
    c.farPlane = farPlane;
    return c;
}
