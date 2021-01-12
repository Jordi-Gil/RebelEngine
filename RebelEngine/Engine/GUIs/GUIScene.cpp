#include "GUIScene.h"

#include "Main/Application.h"

#include "CoreModules/ModuleRender.h"
#include "CoreModules/ModuleWindow.h"
#include "CoreModules/ModuleScene.h"
#include "CoreModules/ModuleEditorCamera.h"

#include "ImGui/IconsFontAwesome5.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_utils.h"
#include "ImGui/imgui_internal.h"

float sceneWidth = 0;
float sceneHeight = 0;

GUIScene::GUIScene(const char* name) { 
	_name = name;
}

void GUIScene::Draw() {

	static float _sceneWindowWidth = ImGui::GetWindowWidth();
	static float _sceneWindowHeight = ImGui::GetWindowHeight();

	App->renderer->Draw(_sceneWindowWidth, _sceneWindowHeight);
	
	std::string wName(ICON_FA_BORDER_ALL " "); wName.append(_name);
	ImGui::Begin(wName.c_str(), &_active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);

	ImGui::BeginMenuBar();
	static bool frustum_culling = false;
	static bool frustum_culling_alg[2] = {false, false};

	static int e = 0;
	if (ImGui::RadioButton("No Frustum Culling", &e, 0)) { App->scene->SetMask(NO_FRUSTUM); }
	ImGui::SameLine();
	if(ImGui::RadioButton("Linear AABB Culling", &e, 1)) { App->scene->SetMask(LINEAR_AABB); }
	ImGui::SameLine();
	if(ImGui::RadioButton("Octree Frustum Culling", &e, 2)) { App->scene->SetMask(OCTREE); }

	ImGui::EndMenuBar();

	_sceneFocused = ImGui::IsWindowFocused();

	ImGui::GetWindowDrawList()->AddImage(
		(void*)(intptr_t)App->renderer->GetViewportTexture(),
		ImVec2(ImGui::GetCursorScreenPos()),
		ImVec2(ImGui::GetCursorScreenPos().x + _sceneWindowWidth, ImGui::GetCursorScreenPos().y + _sceneWindowHeight),
		ImVec2(0, 1),
		ImVec2(1, 0));


	if (_sceneWindowWidth != ImGui::GetWindowWidth() || _sceneWindowHeight != ImGui::GetWindowHeight())
	{
		_sceneWindowWidth = ImGui::GetWindowWidth(); sceneWidth = _sceneWindowWidth;
		_sceneWindowHeight = ImGui::GetWindowHeight(); sceneHeight = _sceneWindowHeight;
		App->editorCamera->WindowResized(_sceneWindowWidth, _sceneWindowHeight);
	}
	/* In progress. Infit mouse rotation
	bool startWrap = ImGui::GetIO().MouseDown[ImGuiMouseButton_Right];

	if (startWrap && sceneFocused) {

		int ptX, ptY;
		SDL_GetMouseState(&ptX, &ptY);
		bool outside = false;

		ImVec2 mousePos;
		mousePos.x = ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();

		ImVec2 mouseStart = ImGui::GetWindowContentRegionMin();
		mouseStart.x += ImGui::GetWindowPos().x;
		mouseStart.y += ImGui::GetWindowPos().y + ImGui::GetScrollY() + ImGui::GetStyle().FramePadding.y;

		if (mousePos.x < 0.005) {
			ptX = mouseStart.x;
			outside = true;
		}

		if (mousePos.y < 0.005) {
			ptY = mouseStart.y;
			outside = true;
		}
	} */
	ImGui::End();
}

void GUIScene::ToggleActive() {
	_active = !_active;
}

void GUIScene::GUI_GetWindowSize(float &width, float &height){
	width = sceneWidth;
	height = sceneHeight;
}