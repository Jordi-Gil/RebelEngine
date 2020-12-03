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

void DrawHierarchy(std::unique_ptr<GameObject>& go) {
	for (auto ptr = go->begin(); ptr < go->end(); ptr++){
	//for (int i = 0; i < go->GetNumChildren(); ++i) {
		if (ptr->get()->GetNumChildren() == 0) {
			ImGui::Selectable(ptr->get()->GetName());		
		}else if (ImGui::TreeNode(ptr->get()->GetName())) {
			DrawHierarchy(*ptr);
			ImGui::TreePop();
		}
	}
}

void GUIHierarchy::Draw() {

	std::string wName(ICON_FA_SITEMAP " "); wName.append(name);
	ImGui::Begin(wName.c_str(), &active, ImGuiWindowFlags_NoCollapse);
	if(ImGui::TreeNode("Hierarchy")) {
		DrawHierarchy(App->scene->root);
		ImGui::TreePop();
	}
	ImGui::End();
}