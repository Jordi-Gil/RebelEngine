#pragma once
#include "CoreModules/ModuleGUI.h"

#include "ImGui/imgui.h"

class GUITerminal : public ModuleGUI {

public:

	GUITerminal(const char *_name);

	~GUITerminal();

	void Draw();

	bool IsActive() { return active; };

	const char* GetName() { return name; }

	void Disable();

#ifdef  _DEBUG
	void ClearOpenGLLog();
#endif

private:

	void Clear();

public:

#ifdef  _DEBUG
	bool deletingOGLLog = false;
#endif

	ImVector<char*> logBuffer;
	ImGuiTextFilter filter;
	bool scrollToBottom = false;
	bool logToClipboard = false;
};

