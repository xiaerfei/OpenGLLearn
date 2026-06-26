//
//  ExerciseRegistry.hpp
//  LearnOpenGL
//
//  练习登记表：所有练习在这里注册，GLView 用数字键 1..N 切换。
//  ★ 你写完一个新练习后，只需在 ExerciseRegistry.cpp 的列表里加一行。
//

#pragma once

#include "Exercise.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

struct ExerciseEntry {
    std::string name;
    std::function<std::unique_ptr<Exercise>()> create;
};

// 返回全局练习列表（顺序即数字键顺序，第 0 个为启动默认）。
const std::vector<ExerciseEntry>& exerciseRegistry();
