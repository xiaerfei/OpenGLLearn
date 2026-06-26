# LearnOpenGL

在 macOS 上用 **OpenGL 4.1 Core Profile** 跟着 [learnopengl-cn](https://learnopengl-cn.github.io/) 写练习的脚手架。
内置一套**双视角观察器**（移植自 Nate Robins 的 OpenGL 教学 Demo，用现代管线重写）：

- **左：观察者视角** —— 轨道相机，画出世界网格、坐标轴，以及当前练习「场景相机」的**视锥体**，让你从外部观察物体。
- **右：场景视角** —— 用练习自己的相机渲染，即"正式画面"。

## 构建与运行

工程由 [XcodeGen](https://github.com/yonaskolb/XcodeGen) 管理，改完结构后重新生成即可（不用手动维护 `.xcodeproj`）：

```bash
brew install xcodegen        # 仅首次
cd LearnOpenGL
xcodegen generate            # 增删源文件 / 改设置后执行
open LearnOpenGL.xcodeproj    # 在 Xcode 里 Run
```

命令行构建（无签名，本地跑）：

```bash
xcodebuild -project LearnOpenGL.xcodeproj -scheme LearnOpenGL \
  -configuration Debug CODE_SIGNING_ALLOWED=NO build
```

## 操作

| 操作 | 效果 |
|------|------|
| 左侧拖拽鼠标 | 旋转观察者相机 |
| 滚轮 | 观察者相机拉近 / 拉远 |
| 数字键 `1` / `2` … | 切换练习（顺序见注册表） |

## 目录结构

```
LearnOpenGL/
├── project.yml                 # XcodeGen 配置（唯一工程来源）
├── ThirdParty/glm/             # vendored glm（-isystem 引入）
└── LearnOpenGL/
    ├── Core/                   # 可复用底层组件，写练习时一般不用动
    │   ├── GLView.{h,mm}       # 4.1 Core 上下文 + 渲染循环 + 输入
    │   ├── Shader.{hpp,cpp}    # 着色器编译/链接/uniform
    │   ├── Mesh.{hpp,cpp}      # VAO/VBO/EBO 封装 + 内置图元
    │   ├── Camera.{hpp,cpp}    # 透视相机
    │   ├── OrbitCamera.{hpp,cpp}        # 观察者轨道相机
    │   ├── DebugDraw.{hpp,cpp}          # 线框：网格/坐标轴/视锥体
    │   ├── DualViewportRenderer.{hpp,cpp}  # 左右双视角编排
    │   └── Exercise.hpp        # 练习基类接口
    └── Exercises/              # ★ 你在这里写练习
        ├── ExerciseRegistry.{hpp,cpp}  # 练习注册表（加练习改这里）
        ├── HelloTriangleExercise.{hpp,cpp}  # 手动 VBO/VAO 的入门模板
        └── CubeExercise.{hpp,cpp}           # 用 Mesh 辅助类的模板
```

## 写一个新练习

1. 在 `Exercises/` 新建 `XxxExercise.{hpp,cpp}`，继承 `Exercise`：

   ```cpp
   class XxxExercise : public Exercise {
       void setup() override;                       // 建 shader/mesh（GL 上下文已就绪）
       void update(float dt, float time) override;  // 每帧逻辑（可选）
       void render(const Camera& camera) override;  // 用传入相机画场景
   };
   ```

   在 `setup()` 里顺便设好 `sceneCamera`（位置/朝向）—— 左侧观察者会自动画出它的视锥体。

2. 在 `ExerciseRegistry.cpp` 里 `#include` 你的头文件，并往列表加一行 `{"名称", 工厂}`。

3. `xcodegen generate` 重新生成，运行后用数字键切到你的练习。

> 对照 learnopengl-cn 写代码时，几乎可以 1:1 照抄网站的 C++（glm + GLSL）；
> 唯一约定：物体放进世界空间、在 `render` 里用传入的 `camera` 的 `view()/projection()` 变换，
> 这样双视角观察器才能工作。详见 `HelloTriangleExercise` 顶部注释。
