#pragma once
#include "Module.h"
#include <GL/glew.h>
#include <string>

class ModuleProgram : public Module
{

public:

    ModuleProgram();

    char* LoadShaderSource(const char* shader_file_name);

    std::string readFile(const char* filePath);

    GLuint CreateProgram(GLuint vtx_shader, GLuint frg_shader);
    GLuint CompileShader(GLuint type, const char* source);

};

