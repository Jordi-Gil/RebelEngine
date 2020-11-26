#include "GUIScene.h"

#include "Main/Application.h"

#include "CoreModules/ModuleRender.h"
#include "CoreModules/ModuleEditorCamera.h"

#include "ImGui/IconsFontAwesome5.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_utils.h"
#include "ImGui/imgui_internal.h"

float sceneWidth = 0;
float sceneHeight = 0;

GUIScene::GUIScene(const char* _name) { 
	name = _name;
}

void GUIScene::Draw() {

	static float _sceneWindowWidth = ImGui::GetWindowWidth();
	static float _sceneWindowHeight = ImGui::GetWindowHeight();

	App->renderer->Draw(_sceneWindowWidth, _sceneWindowHeight);
	
	std::string wName(ICON_FA_BORDER_ALL " "); wName.append(name);
	ImGui::Begin(wName.c_str(), &active, ImGuiWindowFlags_NoCollapse);

	sceneFocused = ImGui::IsWindowFocused();

	ImGui::GetWindowDrawList()->AddImage(
		(void*)(intptr_t)App->renderer->GetViewportTexture(),
		ImVec2(ImGui::GetCursorScreenPos()),
		ImVec2(ImGui::GetCursorScreenPos().x + _sceneWindowWidth,
			ImGui::GetCursorScreenPos().y + _sceneWindowHeight),
		ImVec2(0, 1),
		ImVec2(1, 0));


	if (_sceneWindowWidth != ImGui::GetWindowWidth() || _sceneWindowHeight != ImGui::GetWindowHeight())
	{
		_sceneWindowWidth = ImGui::GetWindowWidth(); sceneWidth = _sceneWindowWidth;
		_sceneWindowHeight = ImGui::GetWindowHeight(); sceneHeight = _sceneWindowHeight;
		App->editorCamera->WindowResized(_sceneWindowWidth, _sceneWindowHeight);
	}

	bool startWrap = ImGui::GetIO().MouseDown[ImGuiMouseButton_Right];

	if (startWrap && sceneFocused) {

		int ptX, ptY;
		SDL_GetMouseState(&ptX, &ptY);
		bool outside = false;

		ImVec2 mousePos;
		mousePos.x = ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();

		if (mousePos.x < 0.005) {
			//TODO:
			outside = true;
		}

	}
	ImGui::End();

}

void GUIScene::ToggleActive() {
	active = !active;
}

void GUIScene::GUI_GetWindowSize(float &width, float &height){
	width = sceneWidth;// _sceneWindowWidth;
	height = sceneHeight; // _sceneWindowHeight;
}