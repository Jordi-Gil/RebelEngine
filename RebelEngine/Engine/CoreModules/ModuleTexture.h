#pragma once

#include "Module.h"
#include <string>

struct TextureInformation {
	unsigned int gl_id = 0;
	std::string path = "";
	unsigned int w, h;
};

class ModuleTexture : public Module {

public:

	ModuleTexture() {}

	~ModuleTexture() {}

	bool Init() override;
	bool CleanUp() override;

	bool LoadFromFBX(const char* path, const char* fbxPath, TextureInformation &info);

	bool LoadTexture(const char *path, TextureInformation &info);
	unsigned int LoadCubeMap(const char *path);

	void SetMinFilter(unsigned i, unsigned texID);
	void SetMagFilter(unsigned i, unsigned texID);
	void SetWrapS(unsigned i, unsigned texID);
	void SetWrapT(unsigned i, unsigned texID);
	void SetWrapR(unsigned i, unsigned texID);

};

