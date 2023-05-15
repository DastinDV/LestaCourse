#pragma once

namespace core {
class VertexBuffer {
public:
  VertexBuffer();
  VertexBuffer(const void *data, unsigned int size);
  ~VertexBuffer();

  void Bind() const;
  void SetData(const void *data, unsigned int size);
  void Unbind() const;

private:
  unsigned int bufferId;
};
} // namespace core