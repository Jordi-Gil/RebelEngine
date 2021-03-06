#include "GUIHierarchy.h"

#include <string>

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/IconsFontAwesome5.h"

#include "GUIProject.h"

#include "CoreModules/ModuleScene.h"
#include "CoreModules/ModuleGUI.h"
#include "CoreModules/ModuleModel.h"

#include "Components/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "GUIInspector.h"

bool popupOpen = false;

bool Is3DModel(const std::string& extension) {

	for (int i = 0; i < MARRAYSIZE(meshFormat); i++) {
		if (_strcmpi(meshFormat[i], extension.c_str()) == 0) return true;
	}
	return false;
}

GUIHierarchy::GUIHierarchy(const char* name) {
	_name = name;
	_dragged = -1;
	_popuped = -1;
	_go_dragged = nullptr;
}

bool IsNotRelative(GameObject& target, GameObject& dragged) {
	
	if (!target.GetParent()) return true;
	else if (target.GetParent() == &dragged) return false;
	else return IsNotRelative(*target.GetParent(), dragged);

}

void GameObjectRelocation(std::unique_ptr<GameObject>&& go, GameObject& new_parent) {

	ComponentTransform* transfrom = (ComponentTransform*) go->GetComponent(ComponentType::kTRANSFORM);
	ComponentTransform* transformFather = (ComponentTransform*) new_parent.GetComponent(ComponentType::kTRANSFORM);

	go->SetParent(&new_parent);
	transfrom->_localMatrix = transformFather->_globalMatrix.Transposed() * transfrom->_globalMatrix; //Update properly
	new_parent.AddChild(std::move(go));

}

void CreatePopup(std::unique_ptr<GameObject>& go) {
	
	App->gui->_inspector->SetFocusedGameObject(*go);

	ImGui::Selectable("Duplicate"); 
	if(ImGui::IsItemHovered()) ImGui::SetTooltip("Disabled, need to be tested");

	if (ImGui::Selectable("Delete")) {
		go->GetParent()->DeleteChild(go);
		App->gui->_inspector->ResetFocusedGameObject();
	}

	ImGui::Separator();
	if (ImGui::Selectable("Create empty")) {
		std::unique_ptr<GameObject> newgo = App->scene->_poolGameObjects.get();
		std::unique_ptr<ComponentTransform> transform = std::make_unique<ComponentTransform>();
		transform->SetOwner(go.get());
		newgo->SetName("GameObject");
		newgo->AddComponent(std::move(transform));
		newgo->SetParent(go.get());
		go->AddChild(std::move(newgo));
	}

}

void GUIHierarchy::DrawHierarchy(GameObject &go) {

	std::vector<std::unique_ptr<GameObject>>& children = go.GetChildren();
	for (uint i = 0; i < children.size(); i++) {

		bool open = false;
		char nodeName[_MAX_FNAME + 38]; // 38 = ## + uuid size
		sprintf(nodeName, "%s##%s", children[i]->GetName(), children[i]->GetUUID().c_str());

		if (children[i]->GetNumChildren() == 0) {
			ImGui::Selectable(nodeName, children[i]->IsSelected());
		}
		else{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			if (children[i]->IsSelected()) {
				flags = flags | ImGuiTreeNodeFlags_Selected;
			}
			open = ImGui::TreeNodeEx(nodeName, flags);
		}

		if (ImGui::BeginPopupContextItem()) {
			CreatePopup(children[i]);
			ImGui::EndPopup();
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			App->gui->_inspector->SetFocusedGameObject(*children[i]);
		}

		if (ImGui::BeginDragDropSource()) {

			ImGui::SetDragDropPayload("hierarchy_move", "", sizeof(const char*));
			_dragged = i;
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
			DrawHierarchy(*children[i]);
			ImGui::TreePop();
		}
	}
}

void GUIHierarchy::Draw() {

	std::string wName(ICON_FA_SITEMAP " "); wName.append(_name);
	ImGui::Begin(wName.c_str(), &_active, ImGuiWindowFlags_NoCollapse);

	ImVec2 size = ImGui::GetWindowSize();
	ImVec2 pos = ImGui::GetWindowPos();

	ImGuiContext& g = *GImGui;
	ImGuiID id = g.CurrentWindow->GetID(wName.c_str());
	
	ImRect rect(pos,ImVec2(size.x + pos.x, size.y + pos.y));

	if (ImGui::BeginDragDropTargetCustom(rect, id)) {

		std::filesystem::path pwd;
		App->gui->_project->GetTexture(pwd);

		const ImGuiPayload* payload_fbx = ImGui::AcceptDragDropPayload("explorer_drag_n_drop");
		if (payload_fbx && Is3DModel(pwd.extension().string())) {

			App->models->LoadModelFromFBX(pwd.string().c_str());

		}

		ImGui::EndDragDropTarget();

	}

	bool open = ImGui::TreeNodeEx("Hierarchy", ImGuiTreeNodeFlags_OpenOnArrow);
	
	//TODO: Review if can be deleted
	if (ImGui::BeginDragDropSource()) { ImGui::EndDragDropSource(); }

	if (ImGui::BeginDragDropTarget() && _dragged != -1) {

		const ImGuiPayload* payload_hierarchy = ImGui::AcceptDragDropPayload("hierarchy_move");
		if (payload_hierarchy) {

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
		DrawHierarchy(*App->scene->_root);
		ImGui::TreePop();
	}

	ImGui::End();
}

void GUIHierarchy::ToggleActive() {
	_active = !_active;
}
