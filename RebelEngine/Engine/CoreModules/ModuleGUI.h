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
	bool Init() override;
	bool Start() override;
	update_status Update() override;
	bool CleanUp() override;

	virtual bool IsActive() const { return active; }
	virtual const char* GetName() const { return name; }
	virtual void ToggleActive() {}
	virtual void Draw() {}

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

