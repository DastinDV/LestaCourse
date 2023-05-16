#pragma once

#include "vertexBuffer.hxx"

namespace core {

class VAO {
private:
  /* data */
public:
  void Bind() const;
  void Unbind() const;

  void SetVertexBuffer(VertexBuffer *VBO);
  VertexBuffer *GetVertexBuffer();
  unsigned int GetID();
  VAO(/* args */);
  ~VAO();

  unsigned int id;
  VertexBuffer *VBO;
};
} // namespace core