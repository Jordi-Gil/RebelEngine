#pragma once
#include "Module.h"
#include <string>

class ModuleProgram : public Module
{

public:

    ModuleProgram();

    bool Start();

    std::string readFile(const char* filePath);
    unsigned int CreateProgram(unsigned int vtx_shader, unsigned int frg_shader);
    unsigned int CompileShader(unsigned int type, const char* source);

    unsigned int GetMainProgram() { return mainProgram; }

    bool CleanUp();

private:

    unsigned int mainProgram = 0;

};

