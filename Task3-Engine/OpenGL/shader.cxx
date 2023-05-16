#include "glad/glad.h"

#include "shader.hxx"
#include <fstream>
#include <sstream>

std::string core::Shader::GetShaderSource(std::string path) {
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

int core::Shader::CreateShader(const std::string &vertexShader,
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

core::Shader::~Shader() { glUseProgram(0); }

void core::Shader::CreateShaderProgramFromFile(const std::string vertex_pth,
                                               const std::string fragment_pth) {

  const std::string pointMovementShaderSource = GetShaderSource(vertex_pth);
  const std::string fragmentShaderSource = GetShaderSource(fragment_pth);

  m_Program = CreateShader(pointMovementShaderSource, fragmentShaderSource);
}

void core::Shader::Use() { glUseProgram(this->m_Program); }
