//
//  Camera.hpp
//  LearnOpenGL
//
//  透视相机：保存位置/朝向/投影参数，产出 view 与 projection 矩阵。纯 C++。
//  既用于练习的"场景相机"，也用于观察者视角（配合轨道控制，见后续 Phase）。
//

#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    glm::vec3 position{0.0f, 0.0f, 3.0f};
    glm::vec3 target{0.0f, 0.0f, 0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};

    float fovYDegrees = 45.0f;
    float aspect = 1.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    glm::mat4 view() const;
    glm::mat4 projection() const;
};
