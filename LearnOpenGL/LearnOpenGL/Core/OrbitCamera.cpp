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
}

void OrbitCamera::zoom(float dDistance) {
    distance = std::clamp(distance + dDistance, 0.5f, 100.0f);
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
