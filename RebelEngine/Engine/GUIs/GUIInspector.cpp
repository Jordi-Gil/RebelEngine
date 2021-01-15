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

	ImGui::SetNextWindowSizeConstraints(ImVec2(500, 100), ImVec2(2000, 2000));
	std::string wName(ICON_FA_INFO " "); wName.append(_name);
	ImGui::Begin(wName.c_str(), &_active, ImGuiWindowFlags_NoCollapse);
	ImVec4 yellow(1.0000f, 0.8275f, 0.4112f, 1.0000f);

	if (_focused_go) {
		for (auto& ct : _focused_go->GetComponents()) {
			ct->OnEditor();
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
