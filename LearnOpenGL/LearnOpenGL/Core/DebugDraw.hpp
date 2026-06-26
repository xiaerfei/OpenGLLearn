//
//  DebugDraw.hpp
//  LearnOpenGL
//
//  用现代管线（着色器 + 动态 VBO）绘制调试线段：世界网格、坐标轴、相机视锥体。
//  这是对 Nate Robins 旧 world-view 中固定管线可视化的现代化重写。
//

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <OpenGL/gl3.h>

#include "Shader.hpp"

class DebugDraw {
public:
    DebugDraw() = default;
    ~DebugDraw();

    DebugDraw(const DebugDraw&) = delete;
    DebugDraw& operator=(const DebugDraw&) = delete;

    void setup(); // 上下文就绪后创建线着色器与动态 VBO

    void begin(); // 清空本帧累积的线段

    void addLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color);
    void addAxes(const glm::mat4& transform, float length); // 在 transform 处画 RGB 三轴
    void addGrid(float halfSize, float step, const glm::vec3& color); // y=0 平面网格

    // 画出某相机的视锥体线框（含位置/朝向），由其 view、projection 推出。
    void addFrustum(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& color);

    void flush(const glm::mat4& viewProj); // 上传并以 GL_LINES 绘制

private:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
    };

    std::vector<Vertex> vertices_;
    Shader shader_;
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
};
