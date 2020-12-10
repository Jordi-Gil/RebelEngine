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

void GameObjectChildrenRelocation(GameObject& go, const ComponentTransform* old_father_comp, const ComponentTransform* new_father_comp) {

	ComponentTransform* comp = (ComponentTransform*)go.GetComponent(type_component::TRANSFORM);
	comp->UpdateTransform(*old_father_comp, *new_father_comp);
	for (auto const& child : go.GetChildren()) {
		GameObjectChildrenRelocation(*child, old_father_comp, new_father_comp);
	}

}

void GameObjectRelocation(GameObject& go, GameObject& new_father) {

	GameObject* old_father = go.GetParent();
	ComponentTransform* comp = (ComponentTransform*)go.GetComponent(type_component::TRANSFORM);
	ComponentTransform* old_father_comp = (ComponentTransform*)old_father->GetComponent(type_component::TRANSFORM);
	ComponentTransform* new_father_comp = (ComponentTransform*)new_father.GetComponent(type_component::TRANSFORM);

	comp->UpdateTransform(*old_father_comp, *new_father_comp);
	for (auto const& child : go.GetChildren()) {
		GameObjectChildrenRelocation(*child, old_father_comp, new_father_comp);
	}
	go.SetParent(&new_father);
	//old_father->RemoveChild();
	//new_father.AddChild(&go);
}

void DrawHierarchy(GameObject &go) {
	for (auto const &child : go.GetChildren()){
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
