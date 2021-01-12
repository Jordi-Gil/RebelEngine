#pragma once
#include "CoreModules/ModuleGUI.h"

#include "ImGui/imgui.h"

class GUITerminal : public ModuleGUI {

public:

	GUITerminal(const char *name);

	~GUITerminal();

	void Draw() override;

	bool IsActive() const override { return _active; };

	const char* GetName() const override { return _name; }

	void ToggleActive() override;

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

