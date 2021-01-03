#pragma once

#include <filesystem>
#include "imgui.h"

#define BIT(x) (1 << x)

namespace ImGui {

	class FileExplorer {

	public:

		FileExplorer();
		std::pair<bool, uint32_t> FileExplorer::DirectoryTreeViewRecursive(const std::filesystem::path& path, uint32_t* count, int* selection_mask);
		void DrawFileTree(std::string directoryPath);

	private:
		std::filesystem::path _pwd;
	};

}