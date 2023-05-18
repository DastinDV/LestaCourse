#pragma once
#include <string>

namespace core {
class Texture {
public:
  void Bind() const;
  void Unbind() const;

  Texture(const std::string &path);
  void LoadTexture(const std::string &path);

  ~Texture();

private:
  unsigned char *data;
  int width;
  int height;
  int nrChannels;

  unsigned int *textureId;
};
}; // namespace core