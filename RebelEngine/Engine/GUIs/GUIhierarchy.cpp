#include "GUIHierarchy.h"

#include <string>
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/IconsFontAwesome5.h"

#include "CoreModules/ModuleScene.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Components/ComponentTransform.h"



GameObject* dragged;
std::string dragged_name;

GUIHierarchy::GUIHierarchy(const char* _name) {
	name = _name;
	dragged = nullptr;
}

void GameObjectChildrenRelocation(GameObject& go) {

	ComponentTransform* comp = (ComponentTransform*)go.GetComponent(type_component::TRANSFORM);
	comp->UpdateGlobalMatrix();
	for (auto const& child : go.GetChildren()) {
		GameObjectChildrenRelocation(*child);
	}

}

void GameObjectRelocation(GameObject& go, GameObject& new_father) {

	GameObject* old_father = go.GetParent();
	ComponentTransform* comp = (ComponentTransform*)go.GetComponent(type_component::TRANSFORM);

	go.SetParent(&new_father);
	comp->UpdateGlobalMatrix();

	for (auto const& child : go.GetChildren()) {
		GameObjectChildrenRelocation(*child);
	}
	std::unique_ptr<GameObject> _go;
	_go.reset(&go);
	new_father.AddChild(std::move(_go));
	old_father->EraseChildrenByName(go.GetName());

}

void DrawHierarchy(GameObject &go) {
	for (auto &child : go.GetChildren()){
		bool open = false;
			if (child->GetNumChildren() == 0) {
				ImGui::Selectable(child->GetName());
			}
			else {
				open = ImGui::TreeNode(child->GetName());
			}

			if (ImGui::BeginDragDropSource()) {

				ImGui::SetDragDropPayload("hierarchy_move", "", sizeof(const char*));
				ImGui::TextUnformatted(child->GetName());
				dragged = child.get();
				ImGui::EndDragDropSource();

			}

			if (ImGui::BeginDragDropTarget()) {	

				ImGui::TextUnformatted(dragged->GetName());
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("hierarchy_move");
				if (payload)
				{
					GameObjectRelocation(*dragged, *child);
				}
				ImGui::EndDragDropTarget();

			}
			if (open) {
				DrawHierarchy(*child);
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
