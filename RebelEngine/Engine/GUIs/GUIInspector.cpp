#include "GUIInspector.h"

#include "Main/Application.h"

#include "CoreModules/ModuleModel.h"
#include "CoreModules/ModuleTexture.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentCamera.h"

#include "ImGui/IconsFontAwesome5.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_utils.h"
#include "ImGui/imgui_internal.h"

#include "Math/float3x3.h"

#include <string>

const std::vector<const char*> FLOAT3_LABELS = { {"X"},{"Y"},{"Z"} };
static float3 position;
static float3 rotation;
static float3 scale;

GUIInspector::GUIInspector(const char* name) {
	_name = name;
}

void GUIInspector::UpdateTransform() {
	ComponentTransform* ct = (ComponentTransform*) _focused_go->GetComponent(type_component::TRANSFORM);
	ct->SetTransform(position, rotation, scale);
	_focused_go->UpdateChildrenTransform();
	if (_focused_go->GetMask() & GO_MASK_CAMERA) {
	
		Quat rotationQuat = ct->GetRotationQuat();
		ComponentCamera* cc = (ComponentCamera*) _focused_go->GetComponent(type_component::CAMERA);
		cc->SetPosition(position);

		float3x3 rotationMatrix = float3x3::FromQuat(rotationQuat);
		cc->SetFront(rotationMatrix * -float3::unitZ);
		cc->SetUp(rotationMatrix * float3::unitY);
	
	}
}

void GUIInspector::DrawDragFloat3(const char* name, float3& vector, float speed) {
	
	char invis[FILENAME_MAX] = "##" ;
	strcat(invis, name);
	ImGui::BeginColumns(invis, 4, ImGuiColumnsFlags_NoBorder | ImGuiColumnsFlags_NoResize);
	{
		ImGui::Text(name);
		for (int i = 0; i < 3; ++i) {
			ImGui::NextColumn();
			ImGui::PushID(&vector[i]);
			ImGui::Text(FLOAT3_LABELS[i]);
			ImGui::SameLine();
			if (ImGui::DragFloat("", &vector[i], speed)) {
				UpdateTransform();
			};
			ImGui::PopID();
		}
	}
	ImGui::EndColumns();
	
}

void GUIInspector::Draw() {

	std::string wName(ICON_FA_INFO " "); wName.append(_name);
	ImGui::Begin(wName.c_str(), &_active, ImGuiWindowFlags_NoCollapse);
	ImVec4 yellow(1.0000f, 0.8275f, 0.4112f, 1.0000f);
	if (_focused_go) {
		if (ImGui::CollapsingHeader("Transform")) {

			ComponentTransform* ct = (ComponentTransform*) _focused_go->GetComponent(type_component::TRANSFORM);

			ImGui::Text(_focused_go->GetName());
			ImGui::Separator();

			position = ct->GetPosition();
			rotation = ct->GetRotation();
			scale = ct->GetScale();

			DrawDragFloat3("Position", position);
			DrawDragFloat3("Rotation", rotation);
			DrawDragFloat3("Scale", scale);
		
		}

		if (ImGui::CollapsingHeader("Textures")) {
			for (auto texture : App->models->textures) {
			
				ImGui::Text("Texture: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%s", texture.second.name.c_str());
				ImGui::Text("Texture Id: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%d", texture.first);
				ImGui::Text("Size: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%d x %d", texture.second.w, texture.second.h);

				ImGui::Image((void *) texture.first, ImVec2(128,128));
			}
		}
	}
	ImGui::End();

}

void GUIInspector::ToggleActive() {
	_active = !_active;
}

void GUIInspector::SetFocusedGameObject(GameObject& focused){
	if (_focused_go) _focused_go->ToggleSelected();
	_focused_go = &focused;
	_focused_go->ToggleSelected();
}
void GUIInspector::UnSetFocusedGameObject() {
	if (_focused_go) _focused_go->ToggleSelected();
	_focused_go = nullptr;
}