#define GLFW_INCLUDE_NONE

#include "exgine/shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace Exgine;

Shader::Shader() {
  spdlog::trace("Shader::Shader()");

  // C-style strings for OpenGL
  const char *vShaderCode = vertexShaderSource.c_str();
  const char *fShaderCode = fragmentShaderSource.c_str();

  // Shader objects (references via ID)
  unsigned int vertex, fragment = 0;

  // Used to check for errors
  int success;
  char infoLog[512];

  // Vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);

  // Show any compilation errors
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    spdlog::error("Failed to compile vertex shader:\n{}", infoLog);
  };

  // Fragment shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);

  // Show any compilation errors
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    spdlog::error("Failed to compile fragment shader:\n{}", infoLog);
  };

  // Create a program
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);

  // Show any linking errors
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    spdlog::error("Failed to link shaders: {}", infoLog);
  }

  // Shader objects are now copied into the program and can be deleted
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  spdlog::trace("Shader::Shader(\"{}\", \"{}\")", vertexPath, fragmentPath);

  // Code buffers
  std::string vertexCode;
  std::string fragmentCode;

  // File streams
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  // Enable exceptions for reading files
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    // Open files into file streams
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);

    // Read file streams into string streams
    std::stringstream vShaderStream, fShaderStream;

    // Stream file contents into string streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    // Close file streams
    vShaderFile.close();
    fShaderFile.close();

    // Convert string streams to strings
    spdlog::debug("Reading vertex shader file: `{}`", vertexPath);
    vertexCode = vShaderStream.str();
    spdlog::debug("Reading fragment shader file: `{}`", fragmentPath);
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure &e) {
    spdlog::error("Current working directory: {}",
                  std::filesystem::current_path().string());
    spdlog::error("Failed to read shader files: `{}` `{}` for reason: `{}`",
                  vertexPath, fragmentPath, e.what());
  }

  // C-style strings for OpenGL
  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  // Shader objects (references via ID)
  unsigned int vertex, fragment = 0;

  // Used to check for errors
  int success;
  char infoLog[512];

  // Vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);

  // Show any compilation errors
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    spdlog::error("Failed to compile vertex shader:\n{}", infoLog);
  };

  // Fragment shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);

  // Show any compilation errors
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    spdlog::error("Failed to compile fragment shader:\n{}", infoLog);
  };

  // Create a program
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);

  // Show any linking errors
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    spdlog::error("Failed to link shaders: {}", infoLog);
  }

  // Shader objects are now copied into the program and can be deleted
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::Cleanup() {
  spdlog::trace("Shader::Cleanup()");

  glDeleteProgram(ID);
}

void Shader::Use() { glUseProgram(ID); }

void Shader::Set(const std::string &name, float value) {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::Set(const std::string &name, int value) {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::Set(const std::string &name, bool value) {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::Set(const std::string &name, GLfloat *value) {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     value);
}

void Shader::Set(const std::string &name, glm::mat4 value) {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(value));
}

void Shader::Set(const std::string &name, glm::vec3 value) {
  glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1,
               glm::value_ptr(value));
}

void Shader::Set(const std::string &name, float *value, int count) {
  switch (count) {
  case 1:
    glUniform1fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    break;
  case 2:
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    break;
  case 3:
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    break;
  case 4:
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    break;
  default:
    spdlog::error("Shader::Set(): Invalid count: {}", count);
    break;
  }
}
