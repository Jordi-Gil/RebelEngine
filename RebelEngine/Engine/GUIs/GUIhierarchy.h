#pragma once
#include "CoreModules/ModuleGUI.h"
class GUIHierarchy : public ModuleGUI
{
	GUIHierarchy();
	
	void Draw();

	bool IsActive() { return active; };
	const char* GetName() { return name; }

	void ToggleActive();
};

