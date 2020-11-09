#pragma once
#include "Module.h"
#include "Utils/Globals.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:

	ModuleRender();
	~ModuleRender();

	bool Init();

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void* GetContext() { return mainContext; }

private:

	void* mainContext = nullptr;

	unsigned int VBO = 0;
	unsigned int program = 0;

};
