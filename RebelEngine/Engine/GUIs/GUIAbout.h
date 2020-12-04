#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIAbout : public ModuleGUI {

public:

	GUIAbout(const char* _name);

	void Draw() override;

	bool IsActive() const override { return active; };

	const char* GetName() const  override { return name; }

	void ToggleActive() override;

};

