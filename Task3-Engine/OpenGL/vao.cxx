#include "vao.hxx"
#include "glad/glad.h"

void core::VAO::Bind() const {
  glBindVertexArray(id);
  if (VBO)
    VBO->Bind();
  // if (texture)
  //   texture->Bind();
}

void core::VAO::Unbind() const {
  glBindVertexArray(0);
  if (VBO)
    VBO->Unbind();
  // if (texture)
  //   texture->Unbind();
}

void core::VAO::SetVertexBuffer(VertexBuffer *VBO) { this->VBO = VBO; }

void core::VAO::SetTexture(Texture *texture) { this->texture = texture; }

core::Texture *core::VAO::GetTexture() { return this->texture; }

core::VertexBuffer *core::VAO::GetVertexBuffer() { return this->VBO; }

unsigned int core::VAO::GetID() { return id; }

core::VAO::VAO() { glGenVertexArrays(1, &id); }

core::VAO::~VAO() { glDeleteVertexArrays(1, &id); }
