#pragma once
#include "Application.h"

#include "CoreModules/ModuleWindow.h"
#include "CoreModules/ModuleRender.h"
#include "CoreModules/ModuleInput.h"
#include "CoreModules/ModuleEditorCamera.h"
#include "CoreModules/ModuleProgram.h"
#include "CoreModules/ModuleDebugDraw.h"
#include "CoreModules/ModuleTexture.h"
#include "CoreModules/ModuleModel.h"

#include "GUIs/GUITerminal.h"
#include "GUIs/GUIConfiguration.h"

#define TIME_PER_FRAME 1000.0f / 60.f // Approx. 60 fps

Application::Application() {

	applicationTimer.start();
#ifdef _DEBUG
	terminalTimer.start();
	logTimer.start();
#endif
	deltaTime = (float)applicationTimer.read();

	// Order matters: they will Init/start/update in this order
	modules.push_back(std::make_unique<ModuleGUI>()); gui = (ModuleGUI*) modules.rbegin()->get();
	modules.push_back(std::make_unique<ModuleWindow>()); window = (ModuleWindow*)modules.rbegin()->get();
	modules.push_back(std::make_unique<ModuleInput>()); input = (ModuleInput*)modules.rbegin()->get();
	modules.push_back(std::make_unique<ModuleTexture>()); texturer = (ModuleTexture*)modules.rbegin()->get();
	modules.push_back(std::make_unique<ModuleRender>()); renderer = (ModuleRender*)modules.rbegin()->get();
	modules.push_back(std::make_unique<ModuleEditorCamera>()); editorCamera = (ModuleEditorCamera*)modules.rbegin()->get();
	modules.push_back(std::make_unique<ModuleProgram>()); program = (ModuleProgram*)modules.rbegin()->get();
	modules.push_back(std::make_unique<ModuleDebugDraw>()); debugDraw = (ModuleDebugDraw*)modules.rbegin()->get();
	modules.push_back(std::make_unique<ModuleModel>()); models = (ModuleModel*)modules.rbegin()->get();

	gui->PreInit();

}

Application::~Application() {

}

bool Application::Init() {

	bool ret = true;

	for (auto it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	for (auto it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Start();

	return ret;
}

update_status Application::Update() {

#ifdef  _DEBUG
	//dump openGL Log - Only in debug, because the log is only traced in debug mode
	if (terminalTimer.read() >= 30000) { App->gui->terminal->deletingOGLLog = true; gui->terminal->ClearOpenGLLog(); terminalTimer.start(); }
#endif
	deltaTime = (float)applicationTimer.read() / 1000.0f;
	applicationTimer.start();

	update_status ret = UPDATE_CONTINUE;

	for (auto it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for (auto it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for (auto it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	Uint32 prevTime = applicationTimer.read();

	if (prevTime < TIME_PER_FRAME)
		SDL_Delay(TIME_PER_FRAME - prevTime);

	App->gui->config->AddFPS(1/deltaTime, applicationTimer.read());

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (auto it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}

void Application::RequestBrowser(const char* _URL) {

	ShellExecute(NULL, "open", _URL, NULL, NULL, SW_SHOWNORMAL);

}