#include "glRenderer.hxx"
#include "glad/glad.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace core {

GlVertex operator+(const GlVertex &l, const GlVertex &r) {
  return GlVertex{l.x + r.x, l.y + r.y, l.z + r.z};
}

GlVertex operator*(const GlVertex &l, float val) {
  return GlVertex{l.x * val, l.y * val, l.z * val};
}

float *ParseVerticies(std::string path, char separator, int &count) {

  std::vector<float> values;

  float *arr = nullptr;

  std::string shaderCode;
  std::ifstream shaderStream(path, std::ios::in);
  if (shaderStream.is_open()) {
    std::stringstream sstr;
    sstr << shaderStream.rdbuf();

    std::string token;
    while (std::getline(sstr, token, separator)) {
      values.push_back(atof(token.c_str()));
    }

  } else
    throw std::runtime_error("Can't find vertex file!");

  arr = new float[values.size()];
  int i = 0;
  for (auto &item : values)
    arr[i++] = item;

  count = values.size();
  return arr;
}

unsigned int *ParseIndexies(std::string path, char separator, int &count) {

  std::vector<int> values;

  unsigned int *arr = nullptr;

  std::string shaderCode;
  std::ifstream shaderStream(path, std::ios::in);
  if (shaderStream.is_open()) {
    std::stringstream sstr;
    sstr << shaderStream.rdbuf();

    std::string token;
    while (std::getline(sstr, token, separator)) {
      values.push_back(atoi(token.c_str()));
    }

  } else
    throw std::runtime_error("Can't find vertex file!");

  arr = new unsigned int[values.size()];
  int i = 0;
  for (auto &item : values)
    arr[i++] = item;

  return arr;
}

std::string GetShaderSource(std::string path) {
  std::string shaderCode;
  std::ifstream shaderStream(path, std::ios::in);
  if (shaderStream.is_open()) {
    std::stringstream sstr;
    sstr << shaderStream.rdbuf();
    shaderCode = sstr.str();
    shaderStream.close();
  } else
    throw std::runtime_error("Can't find shader file!");

  return shaderCode;
}

unsigned int CompileShader(unsigned int type, const std::string &source) {
  unsigned int id = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  GLint success;
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar InfoLog[1024];
    glGetShaderInfoLog(id, sizeof(InfoLog), NULL, InfoLog);
    fprintf(stderr, "Error compiling shader type [%s]: '%s'\n",
            type == GL_VERTEX_SHADER ? "vertex" : "fragment", InfoLog);
  }

  return id;
}

int CreateShader(const std::string &vertexShader,
                 const std::string &fragmentShader) {
  unsigned int programId = glCreateProgram();
  unsigned int vsId = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fsId = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(programId, vsId);
  glAttachShader(programId, fsId);
  glLinkProgram(programId);

  GLint success;
  glGetProgramiv(programId, GL_LINK_STATUS, &success);
  if (success == 0) {
    GLchar ErrorLog[1024];
    glGetProgramInfoLog(programId, sizeof(ErrorLog), NULL, ErrorLog);
    fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
  }

  glValidateProgram(programId);
  glDeleteShader(vsId);
  glDeleteShader(fsId);

  return programId;
}

GlRenderer::GlRenderer() {}

void GlRenderer::SetAttribute(int id, int size, EGlType type, int stride,
                              void *offset) {
  glEnableVertexAttribArray(id);
  glVertexAttribPointer(id, size, GL_FLOAT, GL_FALSE, stride, (void *)offset);
}

void GlRenderer::SetBuffer(VertexBuffer *buffer) {
  this->currentBuffer = buffer;
}

void GlRenderer::DrawPoint(const int size) {
  if (size == 0)
    return;

  glDrawArrays(GL_POINTS, 0, size);
}

void GlRenderer::DrawTriangle(const int size) {

  if (size == 0)
    return;

  glDrawArrays(GL_TRIANGLES, 0, size);
}

} // namespace core
