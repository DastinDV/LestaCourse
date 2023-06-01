#include "vertexBuffer.hxx"
#include "glDebug.hxx"
#include "glad/glad.h"
#include <iostream>

#define glCheckError() glCheckError_(__FILE__, __LINE__);

namespace core {
VertexBuffer::VertexBuffer() {
  glGenBuffers(1, &bufferId);
  glCheckError();
  std::cout << bufferId << std::endl;
}

VertexBuffer::VertexBuffer(const void *data, unsigned int size) {
  glGenBuffers(1, &bufferId);
  glBindBuffer(GL_ARRAY_BUFFER, bufferId);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  glCheckError();
}

void VertexBuffer::Bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, bufferId);
  glCheckError();
}

// Allocation data buffer
void VertexBuffer::SetData(const void *data, unsigned int size) {
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
  glCheckError();
}

// For replacing data without allocation
void VertexBuffer::SetSubData(const void *data, unsigned int size) {
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
  glCheckError();
}

void VertexBuffer::Unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glCheckError();
}

void VertexBuffer::SetElementsCount(unsigned int elementCount) {
  this->elementCount = elementCount;
}

unsigned int VertexBuffer::GetElementsCount() const {
  return this->elementCount;
}

VertexBuffer::~VertexBuffer() {
  glDeleteBuffers(1, &bufferId);
  glCheckError();
}
} // namespace core