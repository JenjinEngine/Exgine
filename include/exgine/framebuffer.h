#pragma once

#include <glad/glad.h>

namespace Exgine {
class Framebuffer {
public:
  Framebuffer();
  ~Framebuffer();

  void Bind();
  void Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

  void Resize(int width, int height);

  GLuint texture;

private:
  GLuint fbo;
  GLuint rbo;
};
} // namespace Exgine
