#pragma once

#define SOL_ALL_SAFETIES_ON 1
#define GLFW_INCLUDE_NONE

#include <glad/glad.h>

#include "exgine/camera.h"
#include "exgine/scene.h"
#include "exgine/shader.h"

namespace Exgine {
class Renderer {
public:
  Renderer();

  void Render(std::shared_ptr<Scene> scene);

  void Cleanup();

private:
  GLuint vao, vbo, ebo;
  bool loaded = false;

  Exgine::Shader shader;
  Exgine::Camera *camera = nullptr;
};
} // namespace Exgine
