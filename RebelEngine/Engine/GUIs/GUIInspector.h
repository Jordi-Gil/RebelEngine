#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIInspector : public ModuleGUI {

public:

	GUIInspector(const char* _name);

	void Draw() override;

	bool IsActive() const override { return active; };
	const char* GetName() const override { return name; }

	void ToggleActive();
};

