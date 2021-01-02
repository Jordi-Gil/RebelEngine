#pragma once

#include "Module.h"

#include<memory>
#include <vector>

class GUIScene;
class GUITerminal;
class GUIInspector;
class GUIConfiguration;
class GUIHierarchy;

class ModuleGUI :  public Module {

public:

	ModuleGUI();

	void PreInit();
	bool Init() override;
	bool Start() override;
	update_status Update() override;
	bool CleanUp() override;

	virtual bool IsActive() const { return _active; }
	virtual const char* GetName() const { return _name; }
	virtual void ToggleActive() {}
	virtual void Draw() {}

private:

	void DrawMainMenu();
	void PhotoshopLikeStyle();

public:

	GUIScene* _scene = nullptr;
	GUITerminal* _terminal = nullptr;
	GUIInspector* _inspector = nullptr;
	GUIConfiguration* _config = nullptr;
	GUIHierarchy* _hierarchy = nullptr;

private:

	std::vector<std::unique_ptr<ModuleGUI>> _windows;

protected:

	bool _active = false;
	const char* _name = "";
};

