#include "GUIHierarchy.h"
#include "ImGui/imgui_impl_sdl.h"
#include "Main/Application.h"
#include "CoreModules/ModuleScene.h"
#include "Main/GameObject.h"
#include <string>
#include "ImGui/IconsFontAwesome5.h"

GUIHierarchy::GUIHierarchy(const char* _name) {
	name = _name;
}

void DrawHierarchy(GameObject &go) {
	for (auto const &children : go.GetChildren()){
		if (children->GetNumChildren() == 0) {
			ImGui::Selectable(children->GetName());
		} else if (ImGui::TreeNode(children->GetName())) {
			DrawHierarchy(*children);
			ImGui::TreePop();
		}
	}
}

void GUIHierarchy::Draw() {

	std::string wName(ICON_FA_SITEMAP " "); wName.append(name);
	ImGui::Begin(wName.c_str(), &active, ImGuiWindowFlags_NoCollapse);
	if(ImGui::TreeNode("Hierarchy")) {
		DrawHierarchy(*App->scene->root);
		ImGui::TreePop();
	}
	ImGui::End();
}

void GUIHierarchy::ToggleActive() {
	active = !active;
}
