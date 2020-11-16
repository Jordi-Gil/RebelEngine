#pragma once

#include "ImGui/IconsFontAwesome5.h"
#include "ImGui/imgui_utils.h"

#include "Globals.h"

class Console {

public:

	Console() {}

	~Console();

	void Clear();
#ifdef  _DEBUG
	void ClearOpenGLLog();
#endif
	void Draw(const char* title, bool* opened = NULL);

public:

	ImVector<char*> logBuffer;
	ImGuiTextFilter filter;
	bool scrollToBottom;
	bool logToClipboard;
};

extern Console* console;