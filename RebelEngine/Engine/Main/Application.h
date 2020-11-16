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
class ModuleEditor;
class ModuleProgram;
class ModuleDebugDraw;

class Application {

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void RequestBrowser(const char* _URL);

public:

	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleEditorCamera* editorCamera = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleProgram* program = nullptr;
	ModuleDebugDraw* debugDraw = nullptr;

	float deltaTime;

#ifdef _DEBUG
	Timer logTimer;
#endif // _DEBUG

private:

	Timer applicationTimer;
#ifdef _DEBUG
	Timer consoleTimer;
#endif // _DEBUG
	std::list<Module*> modules;

};

extern Application* App;
