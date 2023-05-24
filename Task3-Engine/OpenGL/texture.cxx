#include "texture.hxx"
#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include <iostream>
namespace core {
void Texture::Bind(int textureUnit) const {
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, *textureId);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  std::cout << "Texture bounded " << *textureId << std::endl;
}

void Texture::Unbind() const {
  glActiveTexture(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  std::cout << "Texture unbounded " << *textureId << std::endl;
}

Texture::Texture(const std::string &path) {
  textureId = new unsigned int;
  glGenTextures(1, textureId);
  //  glBindTexture(GL_TEXTURE_2D, *textureId);
  std::cout << "Texture generated id = " << *textureId << std::endl;
  LoadTexture(path);
}

void Texture::LoadTexture(const std::string &path) {
  data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::~Texture() {
  glDeleteTextures(1, textureId);
  stbi_image_free(data);
}
} // namespace core
