#pragma once
#include "CoreModules/ModuleGUI.h"

#include "ImGui/imgui_file_explorer.h"

class GUIProject : public ModuleGUI {

public:

	GUIProject(const char* name);

	void Draw() override;

	void ToggleActive() override;

	bool IsActive() const override { return _active; };

	const char* GetName() const override { return _name; }

	void GetTexture(std::filesystem::path& pwd);

private:

	ImGui::FileExplorer _fileExplorer;

};

