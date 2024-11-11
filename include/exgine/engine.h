#pragma once

#define SOL_ALL_SAFETIES_ON 1
#define GLFW_INCLUDE_NONE

#include "exgine/lualoader.h"
#include "exgine/renderer.h"
#include "exgine/scene.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <memory>

namespace Exgine {
class LuaLoader;

class Engine {
public:
  // Initialize the engine with the supplied
  // OpenGL context.
  Engine(GLFWwindow *window);

  // Render the next frame to the supplied
  // OpenGL context.
  void Draw();

  // Clean up the engine and free any resources.
  void Cleanup();

  // Set the scene to be rendered by the engine.
  void SetScene(std::shared_ptr<Scene> scene, bool firstTime = false);

  // Self-referential classes with API. (needs refs to engine);
  Exgine::LuaLoader *luaLoader;

  std::shared_ptr<Scene> scene = nullptr;

private:
  // OpenGL context.
  GLFWwindow *window;

  Exgine::Renderer renderer;
};
} // namespace Exgine
