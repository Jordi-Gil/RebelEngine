#pragma once

#include "Module.h"
#include <string>

struct TextureInformation {
	std::string name;
	unsigned int w, h;
};

class ModuleTexture : public Module {

public:

	ModuleTexture() {}

	~ModuleTexture() {}

	bool Init() override;
	bool CleanUp() override;

	unsigned int loadTexture(const char *path, TextureInformation &info);

	void SetMinFilter(unsigned i, unsigned texID);
	void SetMagFilter(unsigned i, unsigned texID);
	void SetWrapS(unsigned i, unsigned texID);
	void SetWrapT(unsigned i, unsigned texID);
	void SetWrapR(unsigned i, unsigned texID);

};

