#include "GUIHierarchy.h"

#include <string>

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/IconsFontAwesome5.h"

#include "CoreModules/ModuleScene.h"
#include "Components/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "GUIInspector.h"


GUIHierarchy::GUIHierarchy(const char* name) {
	_name = name;
	_dragged = -1;
	_dragged_depth = 0;
	_go_dragged = nullptr;
}

bool IsNotRelative(GameObject& target, GameObject& dragged) {
	
	if (!target.GetParent()) return true;
	else if (target.GetParent() == &dragged) return false;
	else return IsNotRelative(*target.GetParent(), dragged);

}

void GameObjectRelocation(std::unique_ptr<GameObject>&& go, GameObject& new_father) {

	ComponentTransform* comp = (ComponentTransform*)go->GetComponent(type_component::TRANSFORM);
	ComponentTransform* compPadre = (ComponentTransform*)new_father.GetComponent(type_component::TRANSFORM);

	go->SetParent(&new_father);
	comp->_localMatrix = compPadre->_globalMatrix.Transposed() * comp->_globalMatrix;
	new_father.AddChild(std::move(go));

}

void GUIHierarchy::DrawHierarchy(GameObject &go, uint depth) {
	
	std::vector<std::unique_ptr<GameObject>>& children = go.GetChildren();
	for (uint i = 0; i < children.size(); i++) {
		
		bool open = false;
		char nodeName[_MAX_FNAME + 38]; // 38 = ## + uuid size
		sprintf(nodeName, "%s##%s", children[i]->GetName(), children[i]->GetUUID().c_str());
		if (children[i]->GetNumChildren() == 0) {
			ImGui::Selectable(nodeName); //TODO: Add boolean variable to mark selected object
		}
		else {
			open = ImGui::TreeNodeEx(nodeName, ImGuiTreeNodeFlags_OpenOnArrow);
		}

		if (ImGui::IsItemClicked(0)) {
			App->gui->_inspector->SetFocusedGameObject(*children[i]);
		}

		if (ImGui::BeginDragDropSource()) {

			ImGui::SetDragDropPayload("hierarchy_move", "", sizeof(const char*));
			_dragged = i;
			_dragged_depth = depth;
			_go_dragged = &go;
			ImGui::EndDragDropSource();

		}

		if (ImGui::BeginDragDropTarget() && _dragged != -1) {

			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("hierarchy_move");
			if (payload) {

				std::vector<std::unique_ptr<GameObject>>& dragged_children = _go_dragged->GetChildren();
				if (IsNotRelative(*children[i], *dragged_children[_dragged])) {
					GameObjectRelocation(std::move(dragged_children[_dragged]), *children[i]);
					dragged_children.erase(dragged_children.begin() + _dragged);
					if (_go_dragged == &go) {
						if (_dragged < i) --i;
					}
					_dragged = -1;
					_go_dragged = nullptr;
				}
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

	std::string wName(ICON_FA_SITEMAP " "); wName.append(_name);
	ImGui::Begin(wName.c_str(), &_active, ImGuiWindowFlags_NoCollapse);
	bool open = ImGui::TreeNodeEx("Hierarchy", ImGuiTreeNodeFlags_OpenOnArrow);
	
	if (ImGui::BeginDragDropSource()) { ImGui::EndDragDropSource(); }

	if (ImGui::BeginDragDropTarget() && _dragged != -1) {

		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("hierarchy_move");
		if (payload) {

			std::vector<std::unique_ptr<GameObject>>& dragged_children = _go_dragged->GetChildren();
			if (IsNotRelative(*App->scene->_root, *dragged_children[_dragged])) {
				GameObjectRelocation(std::move(dragged_children[_dragged]), *App->scene->_root);
				dragged_children.erase(dragged_children.begin() + _dragged);
				_dragged = -1;
				_go_dragged = nullptr;
			}
		}
		ImGui::EndDragDropTarget();
	}

	if(open) {
		DrawHierarchy(*App->scene->_root, 0);
		ImGui::TreePop();
	}
	ImGui::End();
}

void GUIHierarchy::ToggleActive() {
	_active = !_active;
}
