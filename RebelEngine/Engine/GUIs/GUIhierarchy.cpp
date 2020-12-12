#include "GUIHierarchy.h"

#include <string>
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/IconsFontAwesome5.h"

#include "CoreModules/ModuleScene.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Components/ComponentTransform.h"


GUIHierarchy::GUIHierarchy(const char* _name) {
	name = _name;
	dragged = -1;
	dragged_depth = 0;
	go_dragged = nullptr;
}

void GameObjectChildrenRelocation(GameObject& go) {

	ComponentTransform* comp = (ComponentTransform*)go.GetComponent(type_component::TRANSFORM);
	comp->UpdateGlobalMatrix();
	for (auto const& child : go.GetChildren()) {
		GameObjectChildrenRelocation(*child);
	}

}

void GameObjectRelocation(std::unique_ptr<GameObject>&& go, GameObject& new_father) {

	GameObject* old_father = go->GetParent();
	ComponentTransform* comp = (ComponentTransform*)go->GetComponent(type_component::TRANSFORM);

	go->SetParent(&new_father);
	comp->UpdateGlobalMatrix();

	for (auto const& child : go->GetChildren()) {
		GameObjectChildrenRelocation(*child);
	}

	new_father.AddChild(std::move(go));

}

void GUIHierarchy::DrawHierarchy(GameObject &go, unsigned int depth) {
	
	std::vector<std::unique_ptr<GameObject>>& children = go.GetChildren();
	for (int i = 0; i < children.size(); i++) {
		
		bool open = false;
		if (children[i]->GetNumChildren() == 0) {
			ImGui::Selectable(children[i]->GetName());
		}
		else {
			open = ImGui::TreeNode(children[i]->GetName());
		}

		if (ImGui::BeginDragDropSource()) {

			ImGui::SetDragDropPayload("hierarchy_move", "", sizeof(const char*));
			ImGui::TextUnformatted(children[i]->GetName());
			dragged = i;
			dragged_depth = depth;
			go_dragged = &go;
			ImGui::EndDragDropSource();

		}

		if (ImGui::BeginDragDropTarget() && dragged != -1 && depth <= dragged_depth) {

			ImGui::TextUnformatted(children[i]->GetName());
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("hierarchy_move");
			if (payload) {

				std::vector<std::unique_ptr<GameObject>>& dragged_children = go_dragged->GetChildren();
				GameObjectRelocation(std::move(dragged_children[dragged]), *children[i]);
				dragged_children.erase(dragged_children.begin() + dragged);
				if (go_dragged == &go) {
					if (dragged < i) --i;
				}
				dragged = -1;
				go_dragged = nullptr;
			}
			ImGui::EndDragDropTarget();

		}

		if (open) {
			DrawHierarchy(*children[i], depth+1);
			ImGui::TreePop();
		}
	}
}

void GUIHierarchy::Draw() {

	std::string wName(ICON_FA_SITEMAP " "); wName.append(name);
	ImGui::Begin(wName.c_str(), &active, ImGuiWindowFlags_NoCollapse);
	if(ImGui::TreeNode("Hierarchy")) {
		DrawHierarchy(*App->scene->root, 0);
		ImGui::TreePop();
	}
	ImGui::End();
}

void GUIHierarchy::ToggleActive() {
	active = !active;
}
