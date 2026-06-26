//
//  HelloTriangleExercise.cpp
//  LearnOpenGL
//

#include "HelloTriangleExercise.hpp"

#include "ShaderLibrary.hpp"

namespace {

// 一个三角形：每个顶点 = 位置(x,y,z) + 颜色(r,g,b)。放在世界原点的 XY 平面上。
const float kVertices[] = {
    // 位置              // 颜色
    -0.8f, -0.6f, 0.0f,  1.0f, 0.0f, 0.0f, // 左下 红
     0.8f, -0.6f, 0.0f,  0.0f, 1.0f, 0.0f, // 右下 绿
     0.0f,  0.7f, 0.0f,  0.0f, 0.0f, 1.0f, // 顶部 蓝
};

} // namespace

void HelloTriangleExercise::setup() {
    loadShader(shader_, "hello_triangle"); // Shaders/hello_triangle.vert + .frag

    // ---- 以下 VAO/VBO 设置与 learnopengl-cn 原教程一致 ----
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);

    // 位置属性 location = 0：3 个 float，步长 6 个 float，偏移 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性 location = 1：3 个 float，步长 6 个 float，偏移 3 个 float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 场景相机：正对三角形
    sceneCamera.position = glm::vec3(0.0f, 0.0f, 2.4f);
    sceneCamera.target = glm::vec3(0.0f);
    sceneCamera.fovYDegrees = 45.0f;
}

void HelloTriangleExercise::render(const Camera& camera) {
    shader_.use();
    shader_.setMat4("uModel", glm::mat4(1.0f));
    shader_.setMat4("uView", camera.view());
    shader_.setMat4("uProjection", camera.projection());

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

HelloTriangleExercise::~HelloTriangleExercise() {
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
}
