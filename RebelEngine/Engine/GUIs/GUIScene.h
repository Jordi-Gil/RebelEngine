#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIScene : public ModuleGUI {

public:

	GUIScene(const char * _name);

	void Draw();

	bool IsActive() { return active; };

	void ToggleActive();

	const char* GetName() { return name; }

	static void GUI_GetWindowSize(float& width, float& height);

	bool IsSceneFocused() { return sceneFocused; }

private:

	bool sceneFocused = true;

};

