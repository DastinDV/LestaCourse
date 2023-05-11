#include "glRenderer.hxx"
#include "../glad/include/glad/glad.h"
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

std::string GetShaderSource(std::string path) {
  std::string shaderCode;
  std::ifstream shaderStream(path, std::ios::in);
  if (shaderStream.is_open()) {
    std::stringstream sstr;
    sstr << shaderStream.rdbuf();
    shaderCode = sstr.str();
    shaderStream.close();
  }

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

GlRenderer::GlRenderer() {
  pointBuffer = new VertexBuffer();
  triangleBuffer = new VertexBuffer();
}

void GlRenderer::DrawPoint(std::vector<GlVertex> &pos) {
  if (pos.size() == 0)
    return;

  GLfloat *coordinate;

  int size = pos.size() * 3;
  coordinate = new GLfloat[size];

  int i = 0;
  for (auto &item : pos) {
    coordinate[i++] = item.x;
    coordinate[i++] = item.y;
    coordinate[i++] = item.z;
  }

  pointBuffer->Bind();
  pointBuffer->SetData(coordinate, sizeof(GLfloat) * size);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glDrawArrays(GL_POINTS, 0, size);

  pointBuffer->Unbind();
  delete[] coordinate;
}

void GlRenderer::DrawTriangle(std::vector<GlVertex> &vertecies) {
  GLfloat *coordinate;

  int size = vertecies.size() * 3;
  coordinate = new GLfloat[size];

  int i = 0;
  for (auto &item : vertecies) {
    coordinate[i++] = item.x;
    coordinate[i++] = item.y;
    coordinate[i++] = item.z;
  }

  triangleBuffer->Bind();
  triangleBuffer->SetData(coordinate, sizeof(GLfloat) * size);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glDrawArrays(GL_TRIANGLES, 0, size);

  triangleBuffer->Unbind();
  delete[] coordinate;
}

} // namespace core
