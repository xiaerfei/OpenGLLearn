//
//  Mesh.hpp
//  LearnOpenGL
//
//  顶点网格封装：管理 VAO/VBO/EBO，支持交错顶点属性与可选索引。纯 C++。
//

#pragma once

#include <OpenGL/gl3.h>
#include <vector>
#include <memory>

class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // 上传交错顶点数据。
    // interleaved : 紧密交错的浮点数组
    // attribSizes : 每个顶点属性的分量数（如 {3,3,3} 表示 pos/normal/color），按 location 0,1,2... 绑定
    // indices     : 可空；为空时按 drawArrays 绘制
    void upload(const std::vector<float>& interleaved,
                const std::vector<int>& attribSizes,
                const std::vector<unsigned int>& indices = {});

    void draw(GLenum mode = GL_TRIANGLES) const;

    // 内置图元：单位立方体（边长 1，居中），含逐面法线与逐面颜色。
    static std::unique_ptr<Mesh> createCube();

private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;
    GLsizei vertexCount_ = 0;
    GLsizei indexCount_ = 0;
};
