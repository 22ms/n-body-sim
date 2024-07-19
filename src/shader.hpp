// Source: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h
#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
    // the program ID
    unsigned int ID;
  
    // constructor reads and builds the shader
    Shader(std::string vertexPath, std::string fragmentPath);
    ~Shader();
    // use/activate the shader
    void Use() const;
    // utility uniform functions
    void SetBool(const char* name, bool value) const;  
    void SetInt(const char* name, int value) const;   
    void SetFloat(const char* name, float value) const;
    void SetMat4(const char* name, const glm::mat4 &mat) const;
private:
    void checkCompileErrors(unsigned int shader, std::string type);
};