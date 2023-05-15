#include "elementBuffer.hxx"
#include "glad/glad.h"

namespace core {
ElementBuffer::ElementBuffer(/* args */) { glGenBuffers(1, &id); }

void ElementBuffer::Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); }

void ElementBuffer::Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

void ElementBuffer::SetData(const void *data, unsigned int size) {
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
}

ElementBuffer::~ElementBuffer() { glDeleteBuffers(1, &id); }
} // namespace core