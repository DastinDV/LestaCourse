#include "vertexBuffer.hxx"
#include "../glad/include/glad/glad.h"
#include <iostream>

VertexBuffer::VertexBuffer() {
  glGenBuffers(1, &bufferId);
  std::cout << bufferId << std::endl;
}

VertexBuffer::VertexBuffer(const void *data, unsigned int size) {
  glGenBuffers(1, &bufferId);
  glBindBuffer(GL_ARRAY_BUFFER, bufferId);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const { glBindBuffer(GL_ARRAY_BUFFER, bufferId); }

void VertexBuffer::SetData(const void *data, unsigned int size) {
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &bufferId); }
