#include "texture.hxx"
#include "glDebug.hxx"
#include "glad/glad.h"

#include "../stb_image.h"

#include <iostream>

#define glCheckError() glCheckError_(__FILE__, __LINE__);

namespace core {
void Texture::Bind(int textureUnit) const {
  glActiveTexture(GL_TEXTURE0 +
                  textureUnit); // Возможно стоит перенести в отдельный метод.
  // Чтобы пользователь сам байндил отдельно.
  glCheckError();

  glBindTexture(GL_TEXTURE_2D, *textureId);
  glCheckError();

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glCheckError();

    glGenerateMipmap(GL_TEXTURE_2D);
    glCheckError();
  }
  std::cout << "Texture bounded " << *textureId << std::endl;
}

void Texture::Unbind() const {
  // glActiveTexture(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glCheckError();
  std::cout << "Texture unbounded " << *textureId << std::endl;
}

Texture::Texture(const std::string &path) {
  textureId = new unsigned int;
  glGenTextures(1, textureId);
  glCheckError();
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
  glCheckError();
}
Texture::~Texture() {
  glDeleteTextures(1, textureId);
  glCheckError();
  stbi_image_free(data);
}
} // namespace core
