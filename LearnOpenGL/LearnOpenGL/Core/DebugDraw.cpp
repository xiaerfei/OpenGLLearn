//
//  DebugDraw.cpp
//  LearnOpenGL
//

#include "DebugDraw.hpp"

#include "ShaderLibrary.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <cstddef>

DebugDraw::~DebugDraw() {
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
}

void DebugDraw::setup() {
    loadShader(shader_, "line"); // Shaders/line.vert + .frag

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const void*)offsetof(Vertex, color));

    glBindVertexArray(0);
}

void DebugDraw::begin() {
    vertices_.clear();
}

void DebugDraw::addLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color) {
    vertices_.push_back({a, color});
    vertices_.push_back({b, color});
}

void DebugDraw::addAxes(const glm::mat4& transform, float length) {
    const glm::vec3 o = glm::vec3(transform * glm::vec4(0, 0, 0, 1));
    const glm::vec3 x = glm::vec3(transform * glm::vec4(length, 0, 0, 1));
    const glm::vec3 y = glm::vec3(transform * glm::vec4(0, length, 0, 1));
    const glm::vec3 z = glm::vec3(transform * glm::vec4(0, 0, length, 1));
    addLine(o, x, glm::vec3(0.9f, 0.2f, 0.2f)); // X 红
    addLine(o, y, glm::vec3(0.2f, 0.9f, 0.2f)); // Y 绿
    addLine(o, z, glm::vec3(0.3f, 0.4f, 0.9f)); // Z 蓝
}

void DebugDraw::addGrid(float halfSize, float step, const glm::vec3& color) {
    for (float v = -halfSize; v <= halfSize + 0.001f; v += step) {
        addLine(glm::vec3(v, 0, -halfSize), glm::vec3(v, 0, halfSize), color);
        addLine(glm::vec3(-halfSize, 0, v), glm::vec3(halfSize, 0, v), color);
    }
}

void DebugDraw::addFrustum(const glm::mat4& view, const glm::mat4& projection,
                           const glm::vec3& color) {
    const glm::mat4 invVP = glm::inverse(projection * view);

    // NDC 立方体 8 角：z=-1 近平面，z=+1 远平面（GL 约定）
    const glm::vec3 ndc[8] = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, // near
        {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}, // far
    };

    glm::vec3 world[8];
    for (int i = 0; i < 8; ++i) {
        glm::vec4 w = invVP * glm::vec4(ndc[i], 1.0f);
        world[i] = glm::vec3(w) / w.w;
    }

    auto edge = [&](int a, int b) { addLine(world[a], world[b], color); };
    // 近平面四边
    edge(0, 1); edge(1, 2); edge(2, 3); edge(3, 0);
    // 远平面四边
    edge(4, 5); edge(5, 6); edge(6, 7); edge(7, 4);
    // 连接近-远（四条侧棱）
    edge(0, 4); edge(1, 5); edge(2, 6); edge(3, 7);

    // 相机位置处画一组小坐标轴，体现其朝向
    addAxes(glm::inverse(view), 0.4f);
}

void DebugDraw::flush(const glm::mat4& viewProj) {
    if (vertices_.empty()) {
        return;
    }
    shader_.use();
    shader_.setMat4("uViewProj", viewProj);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(vertices_.size() * sizeof(Vertex)),
                 vertices_.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, (GLsizei)vertices_.size());
    glBindVertexArray(0);
}
