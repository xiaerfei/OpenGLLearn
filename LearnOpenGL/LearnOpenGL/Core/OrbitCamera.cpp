//
//  OrbitCamera.cpp
//  LearnOpenGL
//

#include "OrbitCamera.hpp"

#include <algorithm>
#include <cmath>

void OrbitCamera::orbit(float dYawDegrees, float dPitchDegrees) {
    yaw += dYawDegrees;
    pitch = std::clamp(pitch + dPitchDegrees, -89.0f, 89.0f);
    targetYaw_ = yaw;
    targetPitch_ = pitch;
    targetDistance_ = distance;
}

void OrbitCamera::zoom(float dDistance) {
    distance = std::clamp(distance + dDistance, 0.5f, 100.0f);
    targetDistance_ = distance;
    targetYaw_ = yaw;
    targetPitch_ = pitch;
}

void OrbitCamera::orbitTarget(float dYawDegrees, float dPitchDegrees) {
    if (!hasTarget_) {
        targetYaw_ = yaw;
        targetPitch_ = pitch;
        targetDistance_ = distance;
        hasTarget_ = true;
    }
    targetYaw_ += dYawDegrees;
    targetPitch_ = std::clamp(targetPitch_ + dPitchDegrees, -89.0f, 89.0f);
}

void OrbitCamera::zoomTarget(float dDistance) {
    if (!hasTarget_) {
        targetYaw_ = yaw;
        targetPitch_ = pitch;
        targetDistance_ = distance;
        hasTarget_ = true;
    }
    targetDistance_ = std::clamp(targetDistance_ + dDistance, 0.5f, 100.0f);
}

void OrbitCamera::update(float dt) {
    if (!hasTarget_) return;
    float t = 1.0f - std::exp(-kSmoothSpeed * dt);
    yaw += (targetYaw_ - yaw) * t;
    pitch += (targetPitch_ - pitch) * t;
    distance += (targetDistance_ - distance) * t;
    if (std::abs(yaw - targetYaw_) < 0.001f &&
        std::abs(pitch - targetPitch_) < 0.001f &&
        std::abs(distance - targetDistance_) < 0.001f) {
        yaw = targetYaw_;
        pitch = targetPitch_;
        distance = targetDistance_;
        hasTarget_ = false;
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
