#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIScene : public ModuleGUI {

public:

	GUIScene(const char * _name);

	void Draw();

	void ToggleActive();

	bool IsActive() { return active; };
	bool IsSceneFocused() { return sceneFocused; }

	const char* GetName() { return name; }
	static void GUI_GetWindowSize(float& width, float& height);

private:

	bool sceneFocused = true;

};

