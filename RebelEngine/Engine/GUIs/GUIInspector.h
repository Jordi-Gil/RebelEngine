#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIInspector : public ModuleGUI {

public:

	GUIInspector(const char* _name);

	void Draw() override;

	bool IsActive() override { return active; };
	const char* GetName() { return name; }

	void ToggleActive();
};

