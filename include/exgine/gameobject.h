#pragma once

#include "exgine/datastore.h"
#define SOL_ALL_SAFETIES_ON 1
#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui.h>

namespace Exgine {
struct Transform {
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec2 scale = glm::vec2(1.0f);
  float rotation = 0.0f;
};

class GameObject {
public:
  Transform transform;

  bool hasTexture = false;
  GLuint textureHandle = 0;

  DataStore datastore;

  glm::vec3 colour = glm::vec3(1.0f);

  const glm::vec3 &GetPosition() const { return transform.position; }
  const glm::vec2 &GetScale() const { return transform.scale; }
  float GetRotation() const { return transform.rotation; }
  const glm::vec3 &GetColour() const { return colour; }

  void SetPosition(const glm::vec3 &position) { transform.position = position; }
  void SetScale(const glm::vec2 &scale) { transform.scale = scale; }
  void SetRotation(float rotation) { transform.rotation = rotation; }
  void SetColour(const glm::vec3 &colour) { this->colour = colour; }

  void Translate(const glm::vec3 &translation) {
    transform.position += translation * ImGui::GetIO().DeltaTime;
  }

  void Scale(const glm::vec2 &scale) {
    transform.scale *= scale * ImGui::GetIO().DeltaTime;
  }

  void Rotate(float rotation) {
    transform.rotation += rotation * ImGui::GetIO().DeltaTime;
    transform.rotation = glm::mod(transform.rotation, 360.0f);
  }
};
} // namespace Exgine
