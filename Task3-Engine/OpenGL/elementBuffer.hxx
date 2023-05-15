#pragma once

namespace core {
class ElementBuffer {
private:
  /* data */
public:
  void Bind() const;
  void SetData(const void *data, unsigned int size);
  void Unbind() const;

  ElementBuffer(/* args */);
  ~ElementBuffer();

  unsigned int id;
};
} // namespace core
