#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIScene : public ModuleGUI {

public:

	GUIScene(const char * _name);

	void Draw() override;

	void ToggleActive() override;

	bool IsActive() const override { return active; };

	bool IsSceneFocused() const { return sceneFocused; }

	const char* GetName() const override { return name; }

	static void GUI_GetWindowSize(float& width, float& height);

private:

	bool sceneFocused = true;

};

