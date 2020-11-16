#pragma once
#include "Application.h"

#include "Utils/Console.h"

#include "CoreModules/ModuleWindow.h"
#include "CoreModules/ModuleRender.h"
#include "CoreModules/ModuleInput.h"
#include "CoreModules/ModuleEditorCamera.h"
#include "CoreModules/ModuleEditor.h"
#include "CoreModules/ModuleProgram.h"
#include "CoreModules/ModuleDebugDraw.h"

#define TIME_PER_FRAME 1000.0f / 60.f // Approx. 60 fps

Application::Application() {

	applicationTimer.start();
#ifdef _DEBUG
	consoleTimer.start();
	logTimer.start();
#endif
	deltaTime = (float)applicationTimer.getDeltaTime();

	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(input = new ModuleInput());
	modules.push_back(editorCamera = new ModuleEditorCamera());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(program = new ModuleProgram());
	modules.push_back(debugDraw = new ModuleDebugDraw());
	modules.push_back(editor = new ModuleEditor());
}

Application::~Application() {

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it) {
		delete* it;
	}
}

bool Application::Init() {

	bool ret = true;

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it) {
		ret = (*it)->Start();
	}

	return ret;
}

update_status Application::Update() {

#ifdef  _DEBUG
	//dump openGL Log - Only in debug, because the log is only traced in debug mode
	if (consoleTimer.getDeltaTime() >= 30000) { console->ClearOpenGLLog(); consoleTimer.start(); }
#endif
	deltaTime = (float)applicationTimer.getDeltaTime() / 1000.0f;
	applicationTimer.start();

	update_status ret = UPDATE_CONTINUE;

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for (std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	Uint32 prevTime = applicationTimer.getDeltaTime();

	if (prevTime < TIME_PER_FRAME)
		SDL_Delay(TIME_PER_FRAME - prevTime);

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}

void Application::RequestBrowser(const char* _URL) {

	ShellExecute(NULL, "open", _URL, NULL, NULL, SW_SHOWNORMAL);

}