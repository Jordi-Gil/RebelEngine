#pragma once

#include "Module.h"
#include "Utils/Globals.h"

class ModuleRender : public Module {

public:

	ModuleRender() {}
	~ModuleRender() {}

	bool Init();
	bool Start();
	update_status PostUpdate();
	bool CleanUp();

	void Draw(float width, float height);
	void BindBuffers(float width, float height);

	void* GetContext() { return mainContext; }
	unsigned int GetViewportTexture() { return viewportTex; }

	void SetVSYNC(bool _VSYNC);

public:

	float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

private:

	void* mainContext = nullptr;

	unsigned int FBO = 0;
	unsigned int RBO = 0;
	unsigned int viewportTex = 0;

};
