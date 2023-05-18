#include "texture.hxx"
#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include <iostream>
namespace core {
void Texture::Bind() const {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, *textureId);
  std::cout << "Texture bounded " << std::endl;
}

void Texture::Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

Texture::Texture(const std::string &path) {
  textureId = new unsigned int;
  glGenTextures(1, textureId);
  glBindTexture(GL_TEXTURE_2D, *textureId);
  LoadTexture(path);
}

void Texture::LoadTexture(const std::string &path) {
  unsigned char *data =
      stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
}
Texture::~Texture() { stbi_image_free(data); }
} // namespace core
