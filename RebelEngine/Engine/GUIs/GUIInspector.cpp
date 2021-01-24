#include "GUIInspector.h"

#include <string>

#include "Main/Application.h"

#include "ImGui/IconsFontAwesome5.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_utils.h"
#include "ImGui/imgui_internal.h"

#include "Math/float3x3.h"

#include "CoreModules/ModuleModel.h"
#include "CoreModules/ModuleTexture.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentCamera.h"

static constexpr const char* FLOAT3_LABELS[3] = { {"X"},{"Y"},{"Z"} };

static float3 position;
static float3 rotation;
static float3 scale;

GUIInspector::GUIInspector(const char* name) {
	_name = name;
}

void GUIInspector::Draw() {

	std::string wName(ICON_FA_INFO " "); wName.append(_name);
	ImGui::Begin(wName.c_str(), &_active, ImGuiWindowFlags_NoCollapse);

	if (_focused_go) {

		ImGui::BeginGroup();

		for (auto& ct : _focused_go->GetComponents()) {
			ct->OnEditor();
		}

		ImGui::EndGroup();

		ImGui::Separator();

		ImGui::BeginGroup();
		

		if (ImGui::Button("Add Component")) {
			
		}

		ImGui::EndGroup();

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

void GUIInspector::ResetFocusedGameObject() {
	if (_focused_go) _focused_go->ToggleSelected();
	_focused_go = nullptr;
}
