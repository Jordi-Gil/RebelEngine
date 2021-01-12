#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIAbout : public ModuleGUI {

public:

	GUIAbout(const char* name);

	void Draw() override;

	bool IsActive() const override { return _active; };

	const char* GetName() const  override { return _name; }

	void ToggleActive() override;

};

