#pragma once
#include "Module.h"
#include <string>

class ModuleProgram : public Module {

public:

    ModuleProgram();

    bool Start() override;

    std::string readFile(const char* filePath);
    unsigned int CreateProgram(unsigned int vtx_shader, unsigned int frg_shader);
    unsigned int CompileShader(unsigned int type, const char* source);

    unsigned int GetMainProgram() { return mainProgram; }
    unsigned int GetSkyboxProgram() { return skyboxProgram; }

    bool CleanUp() override;

private:

    unsigned int mainProgram = 0;
    unsigned int skyboxProgram = 0;

};

