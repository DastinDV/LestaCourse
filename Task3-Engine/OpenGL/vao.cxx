#include "vao.hxx"
#include "glad/glad.h"

void core::VAO::Bind() const {
  glBindVertexArray(id);
  if (VBO)
    VBO->Bind();
}

void core::VAO::Unbind() const {
  glBindVertexArray(0);
  if (VBO)
    VBO->Unbind();
}

void core::VAO::SetVertexBuffer(VertexBuffer *VBO) { this->VBO = VBO; }

core::VAO::VAO() { glGenVertexArrays(1, &id); }

core::VAO::~VAO() { glDeleteVertexArrays(1, &id); }
