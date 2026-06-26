//
//  HelloTriangleExercise.hpp
//  LearnOpenGL
//
//  learnopengl-cn《你好，三角形》的框架适配版 —— 写新练习的参考模板。
//
//  与原教程的对照：
//   · VBO/VAO 创建、glVertexAttribPointer 顶点属性配置、着色器编译链接 —— 与原教程完全一致
//     （这里手动管理 VAO/VBO 而非用 Mesh 辅助类，方便你逐行对照网站代码）。
//   · 唯一不同：原教程的三角形直接用 NDC 坐标、无任何矩阵；本框架是"双视角观察器"，
//     需要把物体放进世界空间、经相机矩阵变换，左侧观察者才能从外部观察它。
//     因此这里多了 uModel/uView/uProjection —— 这正是 learnopengl"坐标系统"章节的内容。
//
//  想省去 VBO/VAO 样板时，可改用 Core/Mesh 辅助类，见 CubeExercise。
//

#pragma once

#include "Exercise.hpp"
#include "Shader.hpp"

#include <OpenGL/gl3.h>

class HelloTriangleExercise : public Exercise {
public:
    void setup() override;
    void render(const Camera& camera) override;
    ~HelloTriangleExercise() override;

private:
    Shader shader_;
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
};
