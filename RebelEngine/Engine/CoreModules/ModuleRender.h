#pragma once
#include "Module.h"
#include "Utils/Globals.h"

class ModuleRender : public Module
{
public:

	ModuleRender();
	~ModuleRender();

	bool Init();
	bool Start();

	update_status PostUpdate();

	bool CleanUp();

	void* GetContext() { return mainContext; }

	void Draw(float width, float height);
	void BindBuffers(float width, float height);
	unsigned int GetViewportTexture() { return viewportTex; }

private:

	void* mainContext = nullptr;

	unsigned int FBO = 0;
	unsigned int RBO = 0;
	unsigned int viewportTex = 0;

};
