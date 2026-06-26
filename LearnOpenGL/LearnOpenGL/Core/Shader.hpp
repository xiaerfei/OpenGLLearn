//
//  Shader.hpp
//  LearnOpenGL
//
//  着色器程序封装：从源码字符串编译/链接，提供 uniform 设置。纯 C++，仅在 .mm/.cpp 中使用。
//

#pragma once

#include <string>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // 从顶点/片段着色器源码编译并链接；失败返回 false 并在控制台打印错误日志。
    bool compile(const std::string& vertexSrc, const std::string& fragmentSrc);

    void use() const;
    GLuint program() const { return program_; }

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

private:
    GLint location(const std::string& name) const;

    GLuint program_ = 0;
};
