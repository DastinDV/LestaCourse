#pragma once
#include <string>
#include <unordered_map>

namespace core {
class Shader {
public:
  Shader(std::string vertex_path, const std::string fragment_path) {
    CreateShaderProgramFromFile(vertex_path, fragment_path);
  }

  Shader(){};
  ~Shader();

  void CreateShaderProgramFromFile(const std::string vertex_pth,
                                   const std::string fragment_pth);
  unsigned int GetProgramID();
  void SetUniform1i(int val, const std::string name);
  void SetMatrix4fvUniform(float *mat4fv, const std::string name);
  void SetVec4fvUniform(float *vec4fv, const std::string name);
  void SetMovable(bool val);
  bool IsMovable();

  void Use();

private:
  std::unordered_map<std::string, unsigned int>
      Location_map; // To avoid unnecessary calls to glGetUniformLocation()

  unsigned int m_Program;
  bool m_CompiledShader = false;

  std::string GetShaderSource(std::string path);
  int CreateShader(const std::string &vertexShader,
                   const std::string &fragmentShader);
  unsigned int GetUniformLocation(std::string uniform_name);
  bool isMovable;
};
} // namespace core