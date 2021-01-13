#include "GUIProject.h"

#include "ImGui/imgui_file_explorer.h"

#include "ImGui/IconsForkAwesome.h"
#include "ImGui/IconsFontAwesome5.h"

static constexpr char* path = "Assets";

GUIProject::GUIProject(const char* name) {
	_name = name;
	_fileExplorer = ImGui::FileExplorer();
}

void GUIProject::Draw() {

	std::string wName(ICON_FK_FOLDER " "); wName.append(_name);
	ImGui::Begin(wName.c_str(), &_active, ImGuiWindowFlags_NoCollapse);

	ImGui::Columns(2);

	_fileExplorer.DrawFileTree(path);

	ImGui::NextColumn();

	_fileExplorer.ShowContent();

	ImGui::End();

}

void GUIProject::ToggleActive() {
	_active = !_active;
}
