#pragma once

#include "Module.h"
#include <string>

struct TextureInformation {
	std::string name;
	unsigned int w, h;
};

class ModuleTexture : public Module {

public:

	ModuleTexture();

	~ModuleTexture();

	bool Init();

	unsigned int loadTexture(const char *path, const char* meshPath, TextureInformation &info);

	void SetMinFilter(int i, unsigned texID);
	void SetMagFilter(int i, unsigned texID);
	void SetWrapS(int i, unsigned texID);
	void SetWrapT(int i, unsigned texID);
	void SetWrapR(int i, unsigned texID);
};

