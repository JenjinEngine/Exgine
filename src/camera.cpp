#define GLFW_INCLUDE_NONE

#include "exgine/camera.h"

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

using namespace Exgine;

Camera::Camera(Shader *shader) : shader(shader) {
  spdlog::trace("Camera::Camera({})", (void *)shader);

  rotation = 0;
  zoom = 1;

  position = glm::vec3(0, 0, 0);

  // Set the camera's projection matrix
  Resize(800, 600);
}

void Camera::Resize(int width, int height) {
  float aspectRatio = width / (float)height;

  float inverseZoom =
      1.0f /
      (zoom /
       10); // Invert to make zoom zoom in and also scale everything down...
  this->projection =
      glm::ortho(-aspectRatio * inverseZoom, aspectRatio * inverseZoom,
                 -inverseZoom, inverseZoom, -1.0f, 1.0f);
}

void Camera::Resize(glm::vec2 size) { Resize(size.x, size.y); }

void Camera::Update() {
  view =
      glm::translate(glm::mat4(1.0f), -position) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1));
}

void Camera::SetPosition(const glm::vec3 &position) {
  this->position = position;
}

void Camera::SetRotation(float rotation) { this->rotation = rotation; }

void Camera::SetZoom(float zoom) { this->zoom = zoom; }

void Camera::Translate(const glm::vec3 &translation) {
  position += translation;
}

void Camera::Rotate(float rotation) { this->rotation += rotation; }

void Camera::Zoom(float zoom) { this->zoom += zoom; }

void Camera::SetShader(Shader *shader) { this->shader = shader; }

void Camera::Use() {
  shader->Use();
  shader->Set("u_viewProjection", GetViewProjection());
}

Camera::~Camera() { spdlog::trace("Camera::~Camera()"); }
