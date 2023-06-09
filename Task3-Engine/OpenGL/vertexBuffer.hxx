#pragma once

#ifndef IZ_DECLSPEC
#define IZ_DECLSPEC
#endif

namespace core {
class IZ_DECLSPEC VertexBuffer {
public:
  VertexBuffer();
  VertexBuffer(const void *data, unsigned int size);
  ~VertexBuffer();

  void Bind() const;
  void SetData(const void *data, unsigned int size);
  void SetSubData(const void *data, unsigned int size);
  void Unbind() const;

  void SetElementsCount(unsigned int elementCount);
  unsigned int GetElementsCount() const;

private:
  unsigned int elementCount;
  unsigned int bufferId;
};
} // namespace core