#pragma once

#include "exgine/shader.h"

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Exgine {
// 2D Orthographic Camera
// NOTE: one day hybrid 2D/3D camera?
class Camera {
public:
  Camera(Shader *shader);
  ~Camera();

  void SetShader(Shader *shader);

  void SetPosition(const glm::vec3 &position);
  void SetRotation(float rotation);
  void SetZoom(float zoom);

  void Translate(const glm::vec3 &translation);
  void Rotate(float rotation);
  void Zoom(float zoom);

  void Resize(int width, int height);
  void Resize(glm::vec2 size);

  void Update();
  void Use();

  const glm::vec3 &GetPosition() const { return position; }
  const float GetRotation() const { return rotation; }
  const glm::mat4 GetViewProjection() const { return projection * view; }

  glm::vec3 *GetPositionPointer() { return &position; }
  float *GetZoomPointer() { return &zoom; }
  float *GetRotationPointer() { return &rotation; }
  glm::vec3 *GetBackgroundPointer() { return &background; }

  glm::mat4 *GetProjetionPointer() { return &this->projection; }

private:
  Shader *shader;

  glm::vec3 position = glm::vec3(0, 0, 0);
  float rotation;
  float zoom;

  glm::mat4 projection;
  glm::mat4 view;

  glm::vec3 background = glm::vec3(0.15, 0.15, 0.15);
};
} // namespace Exgine
