//
//  CubeExercise.cpp
//  LearnOpenGL
//

#include "CubeExercise.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace {

const char* kVertexSrc = R"(
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vNormal;
out vec3 vColor;

void main() {
    vNormal = mat3(uModel) * aNormal;
    vColor = aColor;
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
)";

const char* kFragmentSrc = R"(
#version 410 core
in vec3 vNormal;
in vec3 vColor;
out vec4 FragColor;

uniform vec3 uLightDir;

void main() {
    vec3 n = normalize(vNormal);
    float diff = max(dot(n, normalize(-uLightDir)), 0.0);
    vec3 c = vColor * (0.3 + 0.7 * diff);
    FragColor = vec4(c, 1.0);
}
)";

} // namespace

void CubeExercise::setup() {
    shader_ = std::make_unique<Shader>();
    shader_->compile(kVertexSrc, kFragmentSrc);
    cube_ = Mesh::createCube();

    sceneCamera.position = glm::vec3(0.0f, 0.8f, 3.0f);
    sceneCamera.target = glm::vec3(0.0f);
    sceneCamera.fovYDegrees = 45.0f;
}

void CubeExercise::update(float dt, float time) {
    angleDegrees_ = time * 40.0f;
}

void CubeExercise::render(const Camera& camera) {
    if (!shader_ || !cube_) {
        return;
    }
    const glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees_),
                                        glm::normalize(glm::vec3(0.4f, 1.0f, 0.2f)));
    shader_->use();
    shader_->setMat4("uModel", model);
    shader_->setMat4("uView", camera.view());
    shader_->setMat4("uProjection", camera.projection());
    shader_->setVec3("uLightDir", glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f)));
    cube_->draw();
}
