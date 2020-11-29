#pragma once

#include "Module.h"

#include<memory>
#include <vector>

class GUIScene;
class GUITerminal;
class GUIInspector;
class GUIConfiguration;

class ModuleGUI :  public Module {

public:

	ModuleGUI();

	void PreInit();
	bool Init();
	bool Start();
	update_status Update();
	bool CleanUp();

	virtual bool IsActive() { return true; }
	virtual void Disable() { };
	virtual void Draw() { }

private:

	void DrawMainMenu();
	void PhotoshopLikeStyle();

public:

	GUIScene* scene = nullptr;
	GUITerminal* terminal = nullptr;
	GUIInspector* inspector = nullptr;
	GUIConfiguration* config = nullptr;

private:

	std::vector<std::unique_ptr<ModuleGUI>> windows;

protected:

	bool active = false;
	const char* name = "";
};
