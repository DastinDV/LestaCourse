#pragma once

#include "texture.hxx"
#include "vertexBuffer.hxx"

namespace core {

class VAO {
private:
  /* data */
public:
  void Bind() const;
  void Unbind() const;

  void SetVertexBuffer(VertexBuffer *VBO);
  void SetTexture(Texture *texture);
  Texture *GetTexture();
  VertexBuffer *GetVertexBuffer();
  unsigned int GetID();
  VAO(/* args */);
  ~VAO();

  unsigned int id;
  VertexBuffer *VBO;
  Texture *texture = nullptr;
};
} // namespace core