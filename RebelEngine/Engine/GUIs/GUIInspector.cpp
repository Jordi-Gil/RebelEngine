#include "GUIInspector.h"

#include "Main/Application.h"

#include "ImGui/IconsFontAwesome5.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_utils.h"
#include "ImGui/imgui_internal.h"

#include <string>

GUIInspector::GUIInspector(const char* _name) {
	name = _name;
}

void GUIInspector::Draw() {

	std::string wName(ICON_FA_BORDER_ALL " "); wName.append(name);
	ImGui::Begin(wName.c_str(), &active, ImGuiWindowFlags_NoCollapse);

	static ImVec4 translate = ImVec4(1, 1, 1, 1);
	ImGui::DragFloat3("Translate", &translate.x, 0.1);


	ImGui::End();

}

void GUIInspector::Disable() {
	active = false;
}