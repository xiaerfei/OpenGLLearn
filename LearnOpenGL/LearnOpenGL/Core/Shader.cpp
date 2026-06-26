//
//  Shader.cpp
//  LearnOpenGL
//

#include "Shader.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <vector>

namespace {

// 编译单个着色器阶段，失败返回 0。
GLuint compileStage(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len > 1 ? len : 1);
        glGetShaderInfoLog(shader, (GLsizei)log.size(), nullptr, log.data());
        const char* stage = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
        std::fprintf(stderr, "[Shader] %s 编译失败:\n%s\n", stage, log.data());
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

} // namespace

Shader::~Shader() {
    if (program_) {
        glDeleteProgram(program_);
    }
}

bool Shader::compile(const std::string& vertexSrc, const std::string& fragmentSrc) {
    GLuint vs = compileStage(GL_VERTEX_SHADER, vertexSrc);
    GLuint fs = compileStage(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return false;
    }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = GL_FALSE;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len > 1 ? len : 1);
        glGetProgramInfoLog(prog, (GLsizei)log.size(), nullptr, log.data());
        std::fprintf(stderr, "[Shader] 链接失败:\n%s\n", log.data());
        glDeleteProgram(prog);
        return false;
    }

    if (program_) {
        glDeleteProgram(program_);
    }
    program_ = prog;
    return true;
}

void Shader::use() const {
    glUseProgram(program_);
}

GLint Shader::location(const std::string& name) const {
    return glGetUniformLocation(program_, name.c_str());
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(location(name), value ? 1 : 0);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(location(name), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(location(name), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(location(name), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(value));
}
