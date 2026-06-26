//
//  CubeExercise.hpp
//  LearnOpenGL
//
//  最简单的示例练习：一个带方向光、缓慢自转的彩色立方体。
//  既用于验证双视角框架，也作为编写后续 learnopengl-cn 练习的模板。
//

#pragma once

#include "Exercise.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

#include <memory>

class CubeExercise : public Exercise {
public:
    void setup() override;
    void update(float dt, float time) override;
    void render(const Camera& camera) override;

private:
    std::unique_ptr<Shader> shader_;
    std::unique_ptr<Mesh> cube_;
    float angleDegrees_ = 0.0f;
};
