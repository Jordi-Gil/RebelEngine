#pragma once

#include "Module.h"

class ModuleTexture : public Module
{

public:

	ModuleTexture();

	bool Init();

	unsigned int loadTexture(const char *path, const char* meshPath);

};

