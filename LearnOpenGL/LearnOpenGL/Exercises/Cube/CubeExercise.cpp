//
//  CubeExercise.cpp
//  LearnOpenGL
//

#include "CubeExercise.hpp"

#include "ShaderLibrary.hpp"

#include <glm/gtc/matrix_transform.hpp>

void CubeExercise::setup() {
    shader_ = std::make_unique<Shader>();
    loadShader(*shader_, "cube"); // Shaders/cube.vert + .frag
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
