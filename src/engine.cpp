#define GLFW_INCLUDE_NONE

#include "exgine/engine.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

using namespace Exgine;

Engine::Engine(GLFWwindow *window) : window(window) {
  spdlog::trace("Engine::Engine({})", (void *)window);

  if (glfwGetCurrentContext() == nullptr) {
    spdlog::critical("Engine::Engine(): No current OpenGL context, exiting.");
    exit(EXIT_FAILURE);
  }

  // Enable depth testing.
  /*glEnable(GL_DEPTH_TEST);*/

  // Register the error callback.
  glfwSetErrorCallback([](int error, const char *description) {
    spdlog::error("GLFW Error {}: {}", error, description);
  });

  // Register the window resize callback.
  glfwSetFramebufferSizeCallback(this->window,
                                 [](GLFWwindow *window, int width, int height) {
                                   glViewport(0, 0, width, height);
                                 });

  // Self-referential classes.
  luaLoader = new LuaLoader(this);
}

void Engine::Draw() {
  if (this->scene == nullptr) {
    spdlog::warn("Engine::Draw(): scene is nullptr, skipping.");
    return;
  }

  this->renderer.Render(scene);
}

void Engine::Cleanup() {
  spdlog::trace("Engine::Cleanup()");

  this->luaLoader->Cleanup();
  this->renderer.Cleanup();
}

void Engine::SetScene(std::shared_ptr<Scene> scene, bool firstTime) {
  this->scene = scene;

  if (!firstTime)
    this->luaLoader->Ready();
}
