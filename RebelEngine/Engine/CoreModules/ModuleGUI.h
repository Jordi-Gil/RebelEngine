#pragma once
#include "Module.h"

#include<memory>
#include <vector>

class GUIScene;
class GUITerminal;
class GUIInspector;

class ModuleGUI :  public Module
{

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

	GUIScene* scene = nullptr;
	GUITerminal *terminal = nullptr;
	GUIInspector* inspector = nullptr;

private:

	void DrawMainMenu();
	void PhotoshopLikeStyle();

private:

	std::vector<std::unique_ptr<ModuleGUI>> windows;

protected:
	bool active = false;
	const char* name = "";
};

