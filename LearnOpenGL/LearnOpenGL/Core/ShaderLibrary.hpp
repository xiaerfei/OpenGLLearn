//
//  ShaderLibrary.hpp
//  LearnOpenGL
//
//  从 app bundle 的 Shaders/ 目录按名字加载着色器文件并编译。
//  把 bundle 资源解析隔离在这里，使 Shader 保持纯 C++、不依赖 Foundation。
//

#pragma once

#include <string>

class Shader;

// 加载 Shaders/<baseName>.vert 与 Shaders/<baseName>.frag 并编译进 shader。
// 成功返回 true；缺文件或编译失败时打印日志并返回 false。
bool loadShader(Shader& shader, const std::string& baseName);
