#pragma once

#include <list>

#include "Utils/Globals.h"
#include "CoreModules/Module.h"
#include "Utils/Timer.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleEditorCamera;
class ModuleProgram;
class ModuleDebugDraw;
class ModuleTexture;
class ModuleModel;

class ModuleGUI;

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
	ModuleProgram* program = nullptr;
	ModuleDebugDraw* debugDraw = nullptr;
	ModuleTexture* texturer = nullptr;
	ModuleModel* models = nullptr;

	ModuleGUI *gui = nullptr;

	float deltaTime;

#ifdef _DEBUG
	Timer logTimer;
#endif // _DEBUG

private:

	Timer applicationTimer;
#ifdef _DEBUG
	Timer terminalTimer;
#endif // _DEBUG
	std::list<Module*> modules;

};

extern Application* App;
