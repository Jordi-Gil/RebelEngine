#include "GUIScene.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "CoreModules/ModuleRender.h"
#include "CoreModules/ModuleWindow.h"
#include "CoreModules/ModuleScene.h"
#include "CoreModules/ModuleEditorCamera.h"
#include "CoreModules/ModuleGUI.h"


#include "Components/ComponentTransform.h"

#include "ImGui/IconsFontAwesome5.h"
#include "ImGui/IconsForkAwesome.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_utils.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/ImGuizmo.h"

#include "GUIs/GUIInspector.h"

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
		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(500, 30));
		static bool frustum_culling = false;
		static bool frustum_culling_alg[2] = {false, false};

		static int e = 0;
		if (ImGui::RadioButton("No Frustum Culling", &e, 0)) { App->scene->SetMask(NO_FRUSTUM); }
		ImGui::SameLine();
		if(ImGui::RadioButton("Linear AABB Culling", &e, 1)) { App->scene->SetMask(LINEAR_AABB); }
		ImGui::SameLine();
		if(ImGui::RadioButton("Octree Frustum Culling", &e, 2)) { App->scene->SetMask(OCTREE); }

		static int imguiOP = ImGuizmo::OPERATION::TRANSLATE;
		if (ImGui::RadioButton("Translate", &imguiOP, 0)) {}
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", &imguiOP, 1)) {}
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", &imguiOP, 2)) {}

		ImGui::SameLine();
	
		if (ImGui::Button(App->scene->IsPlaying()? ICON_FK_STOP : ICON_FK_PLAY, ImVec2(30.0f, 0.0f))) { App->scene->TogglePlay(); }
		//ImGui::PopStyleVar();
		ImGui::EndMenuBar();
	

	_sceneFocused = ImGui::IsWindowFocused();
	_sceneHovered = ImGui::IsWindowHovered();
	
	_viewportPos =  ImGui::GetCursorScreenPos();
	_viewportSize = ImVec2(_sceneWindowWidth, _sceneWindowHeight);
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
	
	GameObject* focusedGo = App->gui->_inspector->GetFocusedGameObject();
	if (focusedGo != nullptr) {
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, _sceneWindowWidth, _sceneWindowHeight);
		ImGuizmo::SetDrawlist();
		ImGuizmo::Enable(true);
		ImGuizmo::SetOrthographic(false);


		math::float4x4 viewMat;
		App->editorCamera->GetOpenGLMatrix(matrix_type::VIEW_MATRIX, viewMat);
		math::float4x4 projMat;
		App->editorCamera->GetOpenGLMatrix(matrix_type::PROJECTION_MATRIX, projMat);
		ComponentTransform* focusedTransform = (ComponentTransform*)(focusedGo->GetComponent(type_component::TRANSFORM));
		math::float4x4 modelMatrix = focusedTransform->GetLocalMatrix().Transposed();

		ImGuizmo::Manipulate((const float*)&viewMat, (const float*) &projMat, (ImGuizmo::OPERATION)imguiOP, ImGuizmo::MODE::LOCAL, (float*)&modelMatrix);
		if (ImGuizmo::IsUsing()) {
				focusedTransform->SetTransform(modelMatrix.Transposed());
				focusedGo->UpdateChildrenTransform();
		}
	}
	ImGui::End();
}

void GUIScene::ToggleActive() {
	_active = !_active;
}

void GUIScene::GUI_GetWindowSize(float &width, float &height){
	width = sceneWidth;
	height = sceneHeight;
}