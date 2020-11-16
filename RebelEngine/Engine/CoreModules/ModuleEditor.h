#pragma once

#include "Module.h"

class ModuleEditor : public Module
{
public:

	ModuleEditor();

	bool Init();
	bool Start();

	update_status Update();

	bool CleanUp();
	bool IsSceneFocused() { return sceneFocused; }

private:

	void PhotosopLikeStyle();

#pragma region ImGuiWindows
	update_status DrawMainMenu();
	void DrawAbout(bool* show_about);
	void DrawSceneEditor(bool *show_scene_editor);
	void DrawConfiguration(bool* show_config);
#pragma endregion ImGuiWindows

private:
	char* libraries_info;
	unsigned int libSize;
	bool sceneFocused = false;
};