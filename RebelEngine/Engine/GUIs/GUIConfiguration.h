#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIConfiguration : public ModuleGUI
{

public:

	GUIConfiguration(const char* _name);

	void Draw();

	bool IsActive() { return active; };

	const char* GetName() { return name; }

	void Disable();


};

