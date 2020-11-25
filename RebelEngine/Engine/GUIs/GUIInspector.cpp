#include "GUIInspector.h"

#include "Main/Application.h"

#include "CoreModules/ModuleModel.h"
#include "CoreModules/ModuleTexture.h"

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

	std::string wName(ICON_FA_INFO " "); wName.append(name);
	ImGui::Begin(wName.c_str(), &active, ImGuiWindowFlags_NoCollapse);

	ImVec4 yellow(1.0000f, 0.8275f, 0.4112f, 1.0000f);

	if (ImGui::CollapsingHeader("Transform")) {
		ImGui::TextColored(ImVec4(1,0,0,1), "Disabled for this first assigment");
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		static float aux0 = 0;
		ImGui::Text("Position");
			ImGui::Text("X"); ImGui::SameLine(); ImGui::DragFloat("", &aux0);
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::DragFloat("", &aux0);
			ImGui::Text("Z"); ImGui::SameLine(); ImGui::DragFloat("", &aux0); 
		ImGui::Text("Rotation");
			ImGui::Text("X"); ImGui::SameLine(); ImGui::DragFloat("", &aux0);
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::DragFloat("", &aux0);
			ImGui::Text("Z"); ImGui::SameLine(); ImGui::DragFloat("", &aux0);
		ImGui::Text("Scale");
		static float aux1 = 1;
			ImGui::Text("X"); ImGui::SameLine(); ImGui::DragFloat("", &aux1);
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::DragFloat("", &aux1);
			ImGui::Text("Z"); ImGui::SameLine(); ImGui::DragFloat("", &aux1);
		ImGui::PopItemFlag();
	}

	if (ImGui::CollapsingHeader("Geometry")) {
		int i = 0;
		for (auto mesh : App->models->meshes) {
			ImGui::BulletText("Mesh: %d", i++);
			ImGui::Text("#Vertices: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%d", mesh.numVertices);
			ImGui::Text("#Triangles: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%d", mesh.numFaces);
		}
	}

	if (ImGui::CollapsingHeader("Textures")) {
		for (auto texture : App->models->textures) {
			
			ImGui::Text("Texture: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%s", texture.second.name.c_str());
			ImGui::Text("Texture Id: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%d", texture.first);
			ImGui::Text("Size: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%d x %d", texture.second.w, texture.second.h);

			ImGui::Image((void *) texture.first, ImVec2(128,128));
		}
	}

	ImGui::End();

}

void GUIInspector::ToggleActive() {
	active = !active;
}
