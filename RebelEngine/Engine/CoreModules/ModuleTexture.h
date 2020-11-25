#pragma once

#include "Module.h"
#include <string>

struct TextureInformation {
	std::string name;
	unsigned int w, h;
};

class ModuleTexture : public Module
{

public:

	ModuleTexture();

	bool Init();

	unsigned int loadTexture(const char *path, const char* meshPath, TextureInformation &info);

};

