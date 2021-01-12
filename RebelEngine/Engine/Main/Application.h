#pragma once

#include <vector>
#include <memory>

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
class ModuleScene;
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
	ModuleScene* scene = nullptr;
	ModuleGUI *gui = nullptr;
	ModuleSerializer* serializer = nullptr;

	float deltaTime;

#ifdef _DEBUG
	MSTimer logTimer;
#endif // _DEBUG

private:

	MSTimer applicationTimer;
	Uint32 framecount = 0;


#ifdef _DEBUG
	MSTimer terminalTimer;
#endif // _DEBUG
	std::vector<std::unique_ptr< Module >> modules;

};

extern Application* App;
