#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIScene : public ModuleGUI {

public:

	GUIScene(const char * name);

	void Draw() override;

	void ToggleActive() override;

	bool IsActive() const override { return _active; };

	bool IsSceneFocused() const { return _sceneFocused; }
	bool IsSceneHovered() const { return _sceneHovered; }

	const char* GetName() const override { return _name; }

	static void GUI_GetWindowSize(float& width, float& height);

	ImVec2& GetViewportPos() { return _viewportPos; }
	ImVec2& GetViewportSize() { return _viewportSize; }

private:

	bool _sceneFocused = false;
	bool _sceneHovered = false;
	bool _menubarHovered = false;
	ImVec2 _viewportPos;
	ImVec2 _viewportSize;
};

