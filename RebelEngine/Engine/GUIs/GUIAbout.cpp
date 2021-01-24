#include "GUIAbout.h"

#include "Main/Application.h"

#include "CoreModules/ModuleWindow.h"

#include "ImGui/IconsFontAwesome5.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_utils.h"
#include "ImGui/imgui_internal.h"

#include <vector>
#include <fstream>
#include <string>

const std::vector<std::string> MyReadFile(const char* path) {

	std::ifstream ifs(path);
	std::string line;
	std::vector<std::string> res;
	while (getline(ifs, line, '\n')) res.emplace_back(line);
	
	return res;
}

//Can a file be read at compile time? In C++20, but in early versions?
const std::vector<std::string> libs = MyReadFile("Windows/libraries.txt");
const unsigned int size = libs.size();

GUIAbout::GUIAbout(const char* name) {
	_name = name;
}

void GUIAbout::ToggleActive() {
	_active = !_active;
}

void GUIAbout::Draw() {

	ImGui::OpenPopup(_name);

	if (ImGui::BeginPopupModal(_name, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove)) {

		ImGui::SetWindowSize(ImVec2(0, 0));

		ImGui::Image((void*) App->window->texIcon, ImVec2(128,128), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::Text("Version: Rebel Engine"); ImGui::SameLine(); ImGui::Text(VERSION);
		ImGui::Text("Rebel is a game engine programmed entirely in C++.");
		ImGui::Text("Authors:"); ImGui::SameLine(); ImGui::Text("Jordi Gil Gonzalez and Cristian Ferrer Galan");
		if (ImGui::CollapsingHeader("Libraries used")) {
			for (unsigned int i = 0; i < size; i += 3) {
				ImGui::BulletText(""); ImGui::SameLine();  ImGui::TextURL(libs[i].c_str(), libs[i+1].c_str(), 0); ImGui::SameLine(); ImGui::Text(libs[i+2].c_str());
			}
		}
		ImGui::Text("LICENSE: GNU General Public License v3.0");
		ImGui::NewLine();
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 30);
		if (ImGui::Button("Ok")) {
			ImGui::CloseCurrentPopup();
			_active = false;
		}

		ImGui::EndPopup();
	}

}