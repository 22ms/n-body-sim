// Source: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h

#ifndef SHADER_H
#define SHADER_H

class Shader
{
public:
    // the program ID
    unsigned int ID;
  
    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const char* name, bool value) const;  
    void setInt(const char* name, int value) const;   
    void setFloat(const char* name, float value) const;
private:
    void checkCompileErrors(unsigned int shader, const char* type);
};
  
#endif