#include "GUIInspector.h"

#include "Main/Application.h"

#include "CoreModules/ModuleModel.h"
#include "CoreModules/ModuleTexture.h"

#include "Components/ComponentTransform.h"

#include "ImGui/IconsFontAwesome5.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_utils.h"
#include "ImGui/imgui_internal.h"

#include <string>
const std::vector<const char*> FLOAT3_LABELS = { {"X"},{"Y"},{"Z"} };
static float3 position;
static float3 rotation;
static float3 scale;

GUIInspector::GUIInspector(const char* _name) {
	name = _name;
}

void GUIInspector::UpdateTransform() {
	ComponentTransform* ct = (ComponentTransform*)focused_go->GetComponent(type_component::TRANSFORM);
	ct->SetTransform(position, rotation, scale);
	focused_go->UpdateChildrenTransform();
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

	std::string wName(ICON_FA_INFO " "); wName.append(name);
	ImGui::Begin(wName.c_str(), &active, ImGuiWindowFlags_NoCollapse);
	ImVec4 yellow(1.0000f, 0.8275f, 0.4112f, 1.0000f);
	//TODO: Take the real transform model of each mesh.
	if (focused_go) {
		if (ImGui::CollapsingHeader("Transform")) {

			ComponentTransform* ct = (ComponentTransform*) focused_go->GetComponent(type_component::TRANSFORM);

			ImGui::Text(focused_go->GetName());
			ImGui::Separator();

			position = ct->GetPosition();
			rotation = ct->GetRotation();
			scale = ct->GetScale();

			DrawDragFloat3("Position", position);
			DrawDragFloat3("Rotation", rotation);
			DrawDragFloat3("Scale", scale);
		
			ImGui::PopItemFlag();
		}

		if (ImGui::CollapsingHeader("Geometry")) {
			int i = 0;
			//TODO: Get Geometry properly --> GameObject Clicked
			/*
			for (auto mesh : App->models->meshes) {
				ImGui::BulletText("Mesh: %d", i++);
				ImGui::Text("#Vertices: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%d", mesh.numVertices);
				ImGui::Text("#Triangles: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%d", mesh.numFaces);
			}
			*/
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
	active = !active;
}
