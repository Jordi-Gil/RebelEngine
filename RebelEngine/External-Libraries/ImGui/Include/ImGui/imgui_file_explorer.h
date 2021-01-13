#pragma once

#include <filesystem>

#include "imgui.h"


//Modified by Jordi Gil Gonzalez to adapt to Engine
namespace ImGui {

	class FileExplorer {

	public:

		FileExplorer();
		void FileExplorer::DirectoryTreeViewRecursive(const std::filesystem::path& path, int &count);
		void DrawFileTree(std::string directoryPath);
		void ShowContent();

	private:
		std::filesystem::path _pwd;

	};

}