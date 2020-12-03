#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIAbout : public ModuleGUI {

public:

	GUIAbout(const char* _name);

	void Draw() override;

	bool IsActive() override { return active; };

	const char* GetName() { return name; }

	void ToggleActive();

};

