#include "GUIConfiguration.h"
#include "GUIScene.h"

#include "Main/Application.h"

#include "CoreModules/ModuleWindow.h"
#include "CoreModules/ModuleEditorCamera.h"

#include "ImGui/IconsFontAwesome5.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_utils.h"
#include "ImGui/imgui_internal.h"

#include "infoware/version.hpp"
#include "infoware/utils.hpp"


GUIConfiguration::GUIConfiguration(const char* _name) { 
	name = _name;
}

void GUIConfiguration::Draw() {

	std::string wName(ICON_FA_COGS " "); wName.append(name);
	ImGui::Begin(wName.c_str() , &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);

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
	static float hFov = (RADTODEG)*App->editorCamera->GetHorizontalFov();
	static float movSpeed = App->editorCamera->GetMovSpeed();
	static float rotSpeed = App->editorCamera->GetRotSpeed();
	static float zoomSpeed = App->editorCamera->GetZoomSpeed();
	static float znear = App->editorCamera->GetZNear();
	static float zfar = App->editorCamera->GetZFar();
#pragma endregion cameraVars

	static auto quantities = iware::cpu::quantities();
	const auto device_properties = iware::gpu::device_properties();

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
		float clear[3] = {
			App->editorCamera->clearColor.x,
			App->editorCamera->clearColor.y,
			App->editorCamera->clearColor.z,
		};
		if (ImGui::DragFloat3("Background", clear, 0.01, 0, 1)) {
			App->editorCamera->clearColor.x = clear[0];
			App->editorCamera->clearColor.y = clear[1];
			App->editorCamera->clearColor.z = clear[2];
		}
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

		if (ImGui::CollapsingHeader("GPU")) {

			if (device_properties.empty())
				ImGui::TextColored(ImVec4(1,0,0,0), "No detection methods enabled");

			for (auto i = 0u; i < device_properties.size(); ++i) {
				const auto& properties_of_device = device_properties[i];
				ImGui::BulletText("Device #%d:", (i + 1));
				ImGui::Text("Vendor: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%s", iware::gpu::vendor_name(properties_of_device.vendor));
				ImGui::Text("Name: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%s", properties_of_device.name);
				ImGui::Text("RAM size: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%u MB", 
					(((properties_of_device.memory_size >> 20) >> 10)));
				ImGui::Text("Cache size: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%u MB", 
					(((properties_of_device.cache_size >> 10) >> 10)));
				ImGui::Text("Max frequency: "); ImGui::SameLine(); ImGui::TextColored(yellow, "%ll Hz", properties_of_device.max_frequency);
			}
		}
	}

	ImGui::End();

}

void GUIConfiguration::Disable() {
	active = false;
}