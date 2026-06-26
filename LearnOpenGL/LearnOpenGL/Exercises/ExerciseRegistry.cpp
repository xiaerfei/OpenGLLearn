//
//  ExerciseRegistry.cpp
//  LearnOpenGL
//

#include "ExerciseRegistry.hpp"

#include "HelloTriangleExercise.hpp"
#include "CubeExercise.hpp"

const std::vector<ExerciseEntry>& exerciseRegistry() {
    // ★ 新增练习：包含其头文件，并在下面加一行 {名称, 工厂} 即可。数字键按顺序对应。
    static const std::vector<ExerciseEntry> registry = {
        {"Hello Triangle", [] { return std::unique_ptr<Exercise>(new HelloTriangleExercise()); }},
        {"Cube",           [] { return std::unique_ptr<Exercise>(new CubeExercise()); }},
    };
    return registry;
}
