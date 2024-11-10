#include "exgine/renderer.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <spdlog/spdlog.h>

using namespace Exgine;

Renderer::Renderer() {
  spdlog::trace("Renderer::Renderer()");

  const float verts[] = {
      /* Vertices */ /* Texture Coords */
      -0.5f,
      -0.5f,
      0.0f,
      0.0f,
      0.0f, // Bottom-left
      0.5f,
      -0.5f,
      0.0f,
      1.0f,
      0.0f, // Bottom-right
      0.5f,
      0.5f,
      0.0f,
      1.0f,
      1.0f, // Top-right
      -0.5f,
      0.5f,
      0.0f,
      0.0f,
      1.0f // Top-left
  };

  const unsigned int indices[] = {
      0, 1, 2, // First triangle
      2, 3, 0  // Second triangle
  };

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Texture attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  camera = new Camera(&shader);
}

void Renderer::Render(std::shared_ptr<Scene> scene) {
  this->shader.Use();
  this->camera->Use();
  this->camera->Update();

  static int w, h;
  glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);
  camera->Resize(w, h);

  // Render the scene
  for (auto &[name, gameObject] : scene->gameObjects) {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, gameObject->transform.position);

    model = glm::rotate(model, glm::radians(-gameObject->transform.rotation),
                        glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, glm::vec3(gameObject->transform.scale, 1.0f));

    shader.Set("u_model", model);
    shader.Set("u_colour", gameObject->colour);
    shader.Set("u_hasTexture", gameObject->hasTexture);
    shader.Set("u_texture", 0);

    shader.Set("u_viewProjection", camera->GetViewProjection());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
}

void Renderer::Cleanup() {
  spdlog::trace("Renderer::Cleanup()");

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);

  shader.Cleanup();

  delete camera;
}
