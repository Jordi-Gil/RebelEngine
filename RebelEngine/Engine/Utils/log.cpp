#pragma once
#include "Globals.h"
#include "Console.h"

//Based on https://github.com/ocornut/imgui/issues/300
void log(const char file[], int line, const char* type, const char* format, ...) {

	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);

	static char _Tmpbuffer[1024];
	static char _finalbuffer[4096];

	vsprintf(_Tmpbuffer, format, ap);
	va_end(ap);

	if (ImGui::GetCurrentContext() != nullptr) sprintf(_finalbuffer, "%s [%05d] %s\n", type, ImGui::GetFrameCount(), _Tmpbuffer);
	else sprintf(_finalbuffer, "%s [%05d] %s \n", type, 0, _Tmpbuffer);

	console->logBuffer.push_back(strdup(_finalbuffer));

	console->scrollToBottom = true;
}