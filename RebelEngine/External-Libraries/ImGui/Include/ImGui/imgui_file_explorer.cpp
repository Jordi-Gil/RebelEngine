#include "imgui_file_explorer.h"

#include "IconsFontAwesome5.h"

#include "imgui_internal.h"

#include <map>
#include <string>

#define BIT(x) (1 << x)

static std::map<std::string, bool> lookupFolders;

namespace ImGui {

	FileExplorer::FileExplorer() {
		std::filesystem::_Current_path(_pwd);
	}

	std::pair<bool, uint32_t> FileExplorer::DirectoryTreeViewRecursive(const std::filesystem::path& path, uint32_t count, int selection_mask) {

		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

		bool any_node_clicked = false;
		uint32_t node_clicked = 0;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			ImGuiTreeNodeFlags node_flags = base_flags;
			const bool is_selected = (selection_mask & BIT(count)) != 0;
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			std::string name = entry.path().string();

			auto lastSlash = name.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			name = name.substr(lastSlash, name.size() - lastSlash);

			bool entryIsFile = !std::filesystem::is_directory(entry.path());
			bool node_open = false;

			if (!entryIsFile) {
				int aCount = 0;
				for (const auto& e : std::filesystem::directory_iterator(entry.path())) if (e.is_directory()) ++aCount;
				if (aCount > 0) {
					std::string wName(((lookupFolders[name]) ? ICON_FA_FOLDER_OPEN " " :
						(std::filesystem::is_empty(entry.path()) ? ICON_FA_FOLDER_MINUS " " : ICON_FA_FOLDER " ")));
					wName.append(name); wName.append(std::to_string(count));
					node_open = ImGui::TreeNodeEx((void*)(intptr_t)(count), node_flags, wName.c_str());
				}
				else {
					std::string wName(std::filesystem::is_empty(entry.path()) ? ICON_FA_FOLDER_MINUS " " : ICON_FA_FOLDER " ");
					wName.append(name); wName.append(std::to_string(count));
					ImGui::TreeNodeEx((void*)(intptr_t)(count), node_flags | ImGuiTreeNodeFlags_Leaf, wName.c_str());
					ImGui::TreePop();
				}
			}

			if (ImGui::IsItemClicked(0)) {
				node_clicked = count;
				any_node_clicked = true;
			}
			
			++count;

			if (!entryIsFile) {

				if (node_open) {

					lookupFolders[name] = true;
					auto clickState = DirectoryTreeViewRecursive(entry.path(), count, selection_mask);

					if (!any_node_clicked) {
						any_node_clicked = clickState.first;
						node_clicked = clickState.second;
					}

					ImGui::TreePop();
				}
				else { 
					lookupFolders[name] = false; }
			}
		}

		return { any_node_clicked, node_clicked };
	}

	void FileExplorer::DrawFileTree(std::string directoryPath) {

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

		int base_flags = ImGuiTreeNodeFlags_OpenOnArrow;

		static bool open = false;
		
		static int selection_mask = 1;
		const bool is_selected = (selection_mask & 1) != 0;
		if (is_selected)
			base_flags |= ImGuiTreeNodeFlags_Selected;

		std::string wName((open ? ICON_FA_FOLDER_OPEN " " : ICON_FA_FOLDER " "));
		wName.append(directoryPath);
		
		open = ImGui::TreeNodeEx( directoryPath.c_str(), base_flags, "%s", wName.c_str());
		
		if (open) {

			auto clickState = DirectoryTreeViewRecursive(directoryPath, 2, selection_mask);

			if (clickState.first) {
				selection_mask = BIT(clickState.second);
			}

			ImGui::TreePop();
		}

		ImGui::PopStyleVar();
	}
}