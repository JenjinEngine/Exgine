#pragma once

#define SOL_ALL_SAFETIES_ON 1
#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

const std::string vertexShaderSource = R"(
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 u_model;
uniform mat4 u_viewProjection;

out vec2 TexCoord;

void main() {
  gl_Position = u_viewProjection * u_model * vec4(aPos, 1.0);
  TexCoord = aTexCoord;
}
)";

const std::string fragmentShaderSource = R"(
  #version 460 core

  out vec4 FragColor;
  in vec2 TexCoord;

  uniform sampler2D u_texture;
  uniform vec3 u_colour;
  uniform bool u_hasTexture;

  void main() {
    if (u_hasTexture) {
      FragColor = texture(u_texture, TexCoord);
    } else {
      FragColor = vec4(u_colour, 1.0);
    }
    //FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); // testing
  }
)";

namespace Exgine {
// Shader class, helps to load and manage shaders
class Shader {
public:
  unsigned int ID; // ID of the shader program object

  // Takes in the path to the vertex and fragment shaders
  Shader(const std::string &vertexPath, const std::string &fragmentPath);

  // Constructs a shader out of the default vertex and fragment shaders
  Shader();

  void Cleanup();

  // Apply the shader
  void Use();

  // Utility uniform functions for setting values
  // without using many different functions
  void Set(const std::string &name, float value);
  void Set(const std::string &name, int value);
  void Set(const std::string &name, bool value);
  void Set(const std::string &name, GLfloat *value);
  void Set(const std::string &name, glm::mat4 value);
  void Set(const std::string &name, glm::vec3 value);
  void Set(const std::string &name, float *value, int count);
};
} // namespace Exgine
