#include "GUIConfiguration.h"
#include "GUIScene.h"

#include "Main/Application.h"

#include "CoreModules/ModuleWindow.h"
#include "CoreModules/ModuleRender.h"
#include "CoreModules/ModuleModel.h"
#include "CoreModules/ModuleEditorCamera.h"

#include "ImGui/IconsFontAwesome5.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_utils.h"
#include "ImGui/imgui_internal.h"

#include "infoware/version.hpp"
#include "infoware/utils.hpp"

#include <GL\glew.h>

#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

constexpr char* minFilters[6] = { 
	"Nearest",  "Linear", "Nearest Mipmap - Nearest", "Linear Mipmap - Nearest",
	"Nearest Mipmap - Linear", "Linear Mipmap - Linear"
};
constexpr char* magFilters[2] = { "Nearest",  "Linear" };
constexpr char* wrap[5] = { "Clamp to Edge", "Clamp to Border", "Mirrored repeat",
	"Repeat", "Mirror calmp to Edge" };

constexpr int MAX_FRAMES = 60;

iware::cpu::quantities_t quantities = iware::cpu::quantities();

GUIConfiguration::GUIConfiguration(const char* _name) {

	name = _name;

	fpsHist = std::vector<float>(MAX_FRAMES, 0);
	msHist = std::vector<float>(MAX_FRAMES, 0);
}

void GUIConfiguration::Draw() {

	std::string wName(ICON_FA_COGS " "); wName.append(name);
	ImGui::Begin(wName.c_str() , &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);

	static bool vsync = VSYNC;
#pragma region texture
	static const char* min_currentItem = minFilters[1];
	static const char* mag_currentItem = minFilters[1];
	static const char* wrapS_currentItem = wrap[3];
	static const char* wrapT_currentItem = wrap[3];
#pragma endregion texture

#pragma region windowsVars
	int width = App->window->GetCurrentWidth();
	int height = App->window->GetCurrentHeight();
	static float brightness = App->window->GetWindowBrightness();
	static bool fullScreen = FULLSCREEN;
	static bool resizable = RESIZABLE;
	static bool borderless = BORDERLESS;
	static bool fullScreenDesk = FULLDESKTOP;
#pragma endregion windowsVars

#pragma region cameraVars
	static float hFov = RadToDeg(App->editorCamera->GetHorizontalFov());
	static float movSpeed = App->editorCamera->GetMovSpeed();
	static float rotSpeed = App->editorCamera->GetRotSpeed();
	static float zoomSpeed = App->editorCamera->GetZoomSpeed();
	static float znear = App->editorCamera->GetZNear();
	static float zfar = App->editorCamera->GetZFar();
#pragma endregion cameraVars
	
	static bool alphatest = false;
	static bool blend = false;
	static bool cullFace = true;
	static bool depthTest = true;
	static bool scissorTest = false;
	static bool stencilTest = false;

	if (ImGui::CollapsingHeader("Application")) {
		
		ImGui::LabelText("App Name", "%s", TITLE);
		ImGui::LabelText("Organization", "%s", ORGANIZATION);
		ImGui::LabelText("Version", "%s", VERSION);

		if (ImGui::Checkbox("VSYNC", &vsync)) App->renderer->SetVSYNC(vsync);

		ImGui::Separator();
		char title[32]; sprintf_s(title, 32, "Framerate: %.2f", fpsHist[fpsHist.size()-1]);
		ImGui::PlotHistogram("##framerate", &fpsHist[0], fpsHist.size(), 0, title, 0.0, 100.0f, ImVec2(400, 100));
		sprintf_s(title, 32, "Milliseconds: %.2f", msHist[msHist.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &msHist[0], msHist.size(), 0, title, 0.0, 100.0f, ImVec2(400, 100));

	}
	if (ImGui::CollapsingHeader("Window")) {

		if (ImGui::SliderFloat("Brightness", &brightness, 0.2f, 1.0f)) App->window->SetWindowBrightness(brightness);

		if (fullScreen || !resizable || fullScreenDesk) { ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f); }

		if (ImGui::SliderInt("Width", &width, App->window->minWidth, App->window->maxWidth)) App->window->SetWindowWidth(width);
		if (ImGui::SliderInt("Height", &height, App->window->minHeight, App->window->maxHeight)) App->window->SetWindowHeight(height);
		if (fullScreen || !resizable || fullScreenDesk) { ImGui::PopItemFlag(); ImGui::PopStyleVar(); }

		ImGui::Separator();
		ImGui::Text("Refresh rate: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.9922f, 0.5490f, 0.0157f, 1.0000f), "%.2f Hz", App->window->GetWindowRefreshRate());
		ImGui::Text("Frame rate: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.9922f, 0.5490f, 0.0157f, 1.0000f), "%2.f FPS", ImGui::GetIO().Framerate);

		if (ImGui::Checkbox("Fullscreen", &fullScreen)) { App->window->SetWindowFullScreen(fullScreen); fullScreenDesk = false; }
		ImGui::SameLine();
		if (fullScreen || fullScreenDesk) { ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f); }
		if (ImGui::Checkbox("Resizable", &resizable)) App->window->SetWindowResizable(resizable);
		if (ImGui::Checkbox("Borderless", &borderless)) App->window->SetWindowBorderless(borderless);
		if (fullScreen || fullScreenDesk) { ImGui::PopItemFlag(); ImGui::PopStyleVar(); }
		ImGui::SameLine();
		if (ImGui::Checkbox("Fullscreen Desktop", &fullScreenDesk)) { App->window->SetWindowFullScreenDesktop(fullScreenDesk); fullScreen = false; }
	}
	
	if (ImGui::CollapsingHeader("Render")) {

		float clear[3] = {
			App->renderer->clearColor[0],
			App->renderer->clearColor[1],
			App->renderer->clearColor[2],
		};

		if (ImGui::Checkbox("Blend", &blend)) App->renderer->EnableBlend(blend);
		if (ImGui::Checkbox("Cull face", &cullFace)) App->renderer->EnableCullFace(cullFace);
		if (ImGui::Checkbox("Alpha test", &alphatest)) App->renderer->EnableAlphaTest(alphatest);
		if (ImGui::Checkbox("Scissor test", &scissorTest)) App->renderer->EnableScissorTest(scissorTest);
		if (ImGui::Checkbox("Stencil test", &stencilTest)) App->renderer->EnableStencilTest(stencilTest);
		if (ImGui::Checkbox("Depth test", &depthTest)) App->renderer->EnableDepthTest(depthTest);

		if (ImGui::DragFloat3("Background", clear, 0.01, 0, 1)) {
			App->renderer->clearColor[0] = clear[0];
			App->renderer->clearColor[1] = clear[1];
			App->renderer->clearColor[2] = clear[2];
		}

	}

	if (ImGui::CollapsingHeader("Texture")) {
		
		ImGui::PushItemWidth(100);
		
		if (ImGui::BeginCombo("Minification function", min_currentItem)) {

			for (unsigned i = 0; i < IM_ARRAYSIZE(minFilters); i++) {

				bool is_selected = (min_currentItem == minFilters[i]);
				if (ImGui::Selectable(minFilters[i], is_selected)) {
					min_currentItem = minFilters[i];
					App->models->SetMinFilter(i);
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		
		if (ImGui::BeginCombo("Magnification function", mag_currentItem)) {

			for (unsigned i = 0; i < IM_ARRAYSIZE(magFilters); i++) {

				bool is_selected = (mag_currentItem == magFilters[i]);
				if (ImGui::Selectable(magFilters[i], is_selected)){
					mag_currentItem = magFilters[i];
					App->models->SetMagFilter(i);
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo("Wrap parameter coord. S", wrapS_currentItem)) {

			for (unsigned i = 0; i < IM_ARRAYSIZE(wrap); i++) {

				bool is_selected = (wrapS_currentItem == wrap[i]);
				if (ImGui::Selectable(wrap[i], is_selected)) {
					wrapS_currentItem = wrap[i];
					App->models->SetWrapS(i);
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo("Wrap parameter coord. T", wrapT_currentItem)) {

			for (unsigned i = 0; i < IM_ARRAYSIZE(wrap); i++) {

				bool is_selected = (wrapT_currentItem == wrap[i]);
				if (ImGui::Selectable(wrap[i], is_selected)) {
					wrapT_currentItem = wrap[i];
					App->models->SetWrapT(i);
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();

	}
	if (ImGui::CollapsingHeader("Camera")) {

		//View only for editor camera
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		float3 front = App->editorCamera->GetFront(); float FrontAux[3] = {front.x, front.y, front.z};
		float3 up = App->editorCamera->GetFront(); float UpAux[3] = { up.x, up.y, up.z };
		float3 right = App->editorCamera->GetFront(); float RightAux[3] = { right.x, right.y, right.z };
		ImGui::DragFloat3("Front", FrontAux);
		ImGui::DragFloat3("Up", UpAux);
		ImGui::DragFloat3("Right", RightAux);
		ImGui::PopItemFlag();

		if (ImGui::DragFloat("Near Plane", &znear, 0.1f)) App->editorCamera->SetZNear(znear);
		if (ImGui::DragFloat("Far Plane", &zfar, 0.1f)) App->editorCamera->SetZFar(zfar);
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		float ar = App->editorCamera->GetAspectRatio();
		ImGui::DragFloat("Aspect Ratio", &ar);
		ImGui::PopItemFlag();
		if (ImGui::SliderFloat("Movement speed", &movSpeed, 0, 5)) App->editorCamera->SetMovSpeed(movSpeed);
		if (ImGui::SliderFloat("Rotation speed", &rotSpeed, 0, 5)) App->editorCamera->SetRotSpeed(rotSpeed);
		if (ImGui::SliderFloat("Zoom speed", &zoomSpeed, 0, 5)) App->editorCamera->SetZoomSpeed(zoomSpeed);
		
		if (ImGui::SliderFloat("FOV", &hFov, 50, 120)) {
			float sceneWidth, sceneHeight; GUIScene::GUI_GetWindowSize(sceneWidth, sceneHeight);
			App->editorCamera->SetHorizontalFov(hFov, sceneWidth / sceneHeight);
		}

	}
	if (ImGui::CollapsingHeader("Hardware information")) {
		ImVec4 yellow(1.0000f, 0.8275f, 0.4112f, 1.0000f);
#pragma region CPU
		if (ImGui::CollapsingHeader("CPU")) {
			ImGui::Text("Vendor: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%s", iware::cpu::vendor().c_str());
			ImGui::Text("Model: "); ImGui::SameLine();  ImGui::TextColored(yellow, "%s", iware::cpu::model_name().c_str());
			ImGui::Text("Architecture: "); ImGui::SameLine();  ImGui::TextColored(yellow, "%s", iware::cpu::architecture_name(iware::cpu::architecture()));
			ImGui::Text("Endianess: "); ImGui::SameLine();  ImGui::TextColored(yellow, "%s", iware::cpu::endianness_name(iware::cpu::endianness()));
			ImGui::Text("Logical CPUs: "); ImGui::SameLine();  ImGui::TextColored(yellow, "%d", quantities.logical);
			ImGui::Text("Physical CPUs: "); ImGui::SameLine();  ImGui::TextColored(yellow, "%d", quantities.physical);
			ImGui::Text("CPU packages: "); ImGui::SameLine();  ImGui::TextColored(yellow, "%d", quantities.packages);
			if (ImGui::CollapsingHeader(" Instruction set support:")) {
				for (auto&& set : { std::make_pair("3D-now!", iware::cpu::instruction_set_t::s3d_now),  //
									std::make_pair("MMX    ", iware::cpu::instruction_set_t::mmx),      //
									std::make_pair("SSE    ", iware::cpu::instruction_set_t::sse),      //
									std::make_pair("SSE2   ", iware::cpu::instruction_set_t::sse2),     //
									std::make_pair("SSE3   ", iware::cpu::instruction_set_t::sse3),     //
									std::make_pair("AVX    ", iware::cpu::instruction_set_t::avx) }) {
					ImGui::Text("%s:", set.first); ImGui::SameLine();  ImGui::TextColored(yellow, "%s",
						(iware::cpu::instruction_set_supported(set.second) ? "Yes" : "No"));
				}
			}

			if (ImGui::CollapsingHeader("Caches")) {
				for (auto i = 1u; i <= 3; ++i) {
					const auto cache = iware::cpu::cache(i);

					ImGui::BulletText("L%d:", i);
					ImGui::Text("Size: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%d", cache.size);
					ImGui::Text("Line size: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%d", cache.line_size);
					ImGui::Text("Associatitvity: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%d", cache.associativity);
					ImGui::Text("Type: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%s", iware::cpu::cache_type_name(cache.type));
				}

			}
		}
#pragma endregion CPU

		ImGui::Separator();

#pragma region GPU

		if (ImGui::CollapsingHeader("GPU")) {

			GLint totalMem = 0;
			glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
				&totalMem);

			GLint availMem = 0;
			glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
				&availMem);

			GLint usageMem = totalMem - availMem;
		
			ImGui::Text("Vendor: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%s", glGetString(GL_VENDOR));
			ImGui::Text("Name: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%s", glGetString(GL_RENDERER));
			ImGui::Text("VRAM size: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%.1f GB", 
				float(totalMem) / (1024.f * 1024.f));
			ImGui::Text("VRAM Available: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%.1f GB", 
				float(availMem) / (1024.f * 1024.f));
			ImGui::Text("VRAM Usage: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%.1f GB",
				float(usageMem) / (1024.f * 1024.f));
			
		}
#pragma endregion GPU
	}

	ImGui::End();

}

void GUIConfiguration::ToggleActive() {
	active = !active;
}

void GUIConfiguration::AddFPS(float FPS, float ms) {

	for (unsigned i = 0; i < MAX_FRAMES - 1; i++) {
		fpsHist[i] = fpsHist[i + 1];
		msHist[i] = msHist[i + 1];
	}

	fpsHist[MAX_FRAMES-1] = FPS;
	msHist[MAX_FRAMES - 1] = ms;

}