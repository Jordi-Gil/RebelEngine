#pragma once
#include "Globals.h"

#include "Main/Application.h"
#include "GUIs/GUITerminal.h"

//Based on https://github.com/ocornut/imgui/issues/300
void log(const char file[], int line, const char* type, const char* format, ...) {

	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);

	static char _Tmpbuffer[1024];
	static char _finalbuffer[4096];

	vsprintf_s(_Tmpbuffer, 1024, format, ap);
	va_end(ap);

	if (ImGui::GetCurrentContext() != nullptr) sprintf_s(_finalbuffer, 4096, "%s [%05d] %s\n", type, ImGui::GetFrameCount(), _Tmpbuffer);
	else sprintf_s(_finalbuffer, 4096, "%s [%05d] %s \n", type, 0, _Tmpbuffer);

	App->gui->terminal->logBuffer.push_back(_strdup(_finalbuffer));
	App->gui->terminal->scrollToBottom = true;
}