#pragma once
#include "CoreModules/ModuleGUI.h"

class GUIHierarchy : public ModuleGUI {

public:

	GUIHierarchy(const char* _name);

	void Draw() override;

	bool IsActive() override { return active; };
	const char* GetName() { return name; }

	void ToggleActive() {};
};
