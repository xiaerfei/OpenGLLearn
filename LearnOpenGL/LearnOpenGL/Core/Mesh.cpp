//
//  Mesh.cpp
//  LearnOpenGL
//

#include "Mesh.hpp"

#include <numeric>

Mesh::~Mesh() {
    if (ebo_) glDeleteBuffers(1, &ebo_);
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
}

void Mesh::upload(const std::vector<float>& interleaved,
                  const std::vector<int>& attribSizes,
                  const std::vector<unsigned int>& indices) {
    const int strideFloats = std::accumulate(attribSizes.begin(), attribSizes.end(), 0);
    if (strideFloats <= 0) {
        return;
    }

    vertexCount_ = (GLsizei)(interleaved.size() / strideFloats);
    indexCount_ = (GLsizei)indices.size();

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(interleaved.size() * sizeof(float)),
                 interleaved.data(), GL_STATIC_DRAW);

    if (!indices.empty()) {
        glGenBuffers(1, &ebo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     (GLsizeiptr)(indices.size() * sizeof(unsigned int)),
                     indices.data(), GL_STATIC_DRAW);
    }

    const GLsizei stride = (GLsizei)(strideFloats * sizeof(float));
    size_t offsetFloats = 0;
    for (GLuint loc = 0; loc < attribSizes.size(); ++loc) {
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, attribSizes[loc], GL_FLOAT, GL_FALSE, stride,
                              (const void*)(offsetFloats * sizeof(float)));
        offsetFloats += attribSizes[loc];
    }

    glBindVertexArray(0);
}

void Mesh::draw(GLenum mode) const {
    if (!vao_) {
        return;
    }
    glBindVertexArray(vao_);
    if (indexCount_ > 0) {
        glDrawElements(mode, indexCount_, GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(mode, 0, vertexCount_);
    }
    glBindVertexArray(0);
}

std::unique_ptr<Mesh> Mesh::createCube() {
    // 6 个面，每面 4 顶点；逐面法线 + 逐面颜色。属性布局 pos(3)/normal(3)/color(3)。
    struct Face {
        float normal[3];
        float color[3];
        float corners[4][3]; // 从面外侧看为逆时针
    };

    const Face faces[6] = {
        // +Z 前 (品红)
        {{ 0, 0, 1}, {0.9f, 0.2f, 0.7f},
         {{-0.5f,-0.5f, 0.5f}, { 0.5f,-0.5f, 0.5f}, { 0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}}},
        // -Z 后 (青)
        {{ 0, 0,-1}, {0.2f, 0.8f, 0.8f},
         {{ 0.5f,-0.5f,-0.5f}, {-0.5f,-0.5f,-0.5f}, {-0.5f, 0.5f,-0.5f}, { 0.5f, 0.5f,-0.5f}}},
        // +X 右 (红)
        {{ 1, 0, 0}, {0.9f, 0.3f, 0.3f},
         {{ 0.5f,-0.5f, 0.5f}, { 0.5f,-0.5f,-0.5f}, { 0.5f, 0.5f,-0.5f}, { 0.5f, 0.5f, 0.5f}}},
        // -X 左 (绿)
        {{-1, 0, 0}, {0.3f, 0.8f, 0.3f},
         {{-0.5f,-0.5f,-0.5f}, {-0.5f,-0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f,-0.5f}}},
        // +Y 上 (蓝)
        {{ 0, 1, 0}, {0.3f, 0.5f, 0.9f},
         {{-0.5f, 0.5f, 0.5f}, { 0.5f, 0.5f, 0.5f}, { 0.5f, 0.5f,-0.5f}, {-0.5f, 0.5f,-0.5f}}},
        // -Y 下 (黄)
        {{ 0,-1, 0}, {0.9f, 0.8f, 0.2f},
         {{-0.5f,-0.5f,-0.5f}, { 0.5f,-0.5f,-0.5f}, { 0.5f,-0.5f, 0.5f}, {-0.5f,-0.5f, 0.5f}}},
    };

    std::vector<float> verts;
    std::vector<unsigned int> indices;
    verts.reserve(6 * 4 * 9);
    indices.reserve(6 * 6);

    for (int f = 0; f < 6; ++f) {
        const Face& face = faces[f];
        unsigned int base = (unsigned int)(f * 4);
        for (int c = 0; c < 4; ++c) {
            verts.insert(verts.end(), {face.corners[c][0], face.corners[c][1], face.corners[c][2]});
            verts.insert(verts.end(), {face.normal[0], face.normal[1], face.normal[2]});
            verts.insert(verts.end(), {face.color[0], face.color[1], face.color[2]});
        }
        indices.insert(indices.end(), {base, base + 1, base + 2, base + 2, base + 3, base});
    }

    auto mesh = std::make_unique<Mesh>();
    mesh->upload(verts, {3, 3, 3}, indices);
    return mesh;
}
