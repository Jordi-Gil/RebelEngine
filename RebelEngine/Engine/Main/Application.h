#pragma once

#include<list>
#include "Utils/Globals.h"
#include "CoreModules/Module.h"
#include "Utils/Timer.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleEditorCamera;

class Application {

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:

	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleEditorCamera* editorCamera = nullptr;

	float deltaTime;

private:

	Timer applicationTimer;
	std::list<Module*> modules;

};

extern Application* App;