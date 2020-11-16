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
	bool Start();

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

	bool CleanUp();

	void* GetContext() { return mainContext; }

	void Draw(float width, float height);

private:

	void* mainContext = nullptr;

	unsigned int VBO = 0;

};
