#include "GUIAbout.h"

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

const std::vector<std::string> libs = MyReadFile("Windows/libraries.txt");
const unsigned int size = libs.size();

GUIAbout::GUIAbout(const char* _name) {
	name = _name;
}

void GUIAbout::Disable() {
	active = false;
}

void GUIAbout::Draw() {

	ImGui::OpenPopup(name);

	if (ImGui::BeginPopupModal(name, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove)) {

		ImGui::SetWindowSize(ImVec2(0, 0));

		ImGui::Text("Version: Rebel Engine"); ImGui::SameLine(); ImGui::Text(VERSION);
		ImGui::Text("Rebel is a game engine programmed entirely in C++.");
		ImGui::Text("Author:"); ImGui::SameLine(); ImGui::Text("Jordi Gil Gonzalez");
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
			active = false;
		}

		ImGui::EndPopup();
	}

}