//
//  Camera.cpp
//  LearnOpenGL
//

#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::view() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::projection() const {
    return glm::perspective(glm::radians(fovYDegrees), aspect, nearPlane, farPlane);
}
