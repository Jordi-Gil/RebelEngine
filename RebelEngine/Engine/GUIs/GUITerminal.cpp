#include "GUITerminal.h"

#include <map>
#include <string>

#include "ImGui/IconsFontAwesome5.h"
#include "ImGui/imgui_utils.h"

GUITerminal::GUITerminal(const char* _name) {
	name = _name;
}

GUITerminal::~GUITerminal() {
	for (int i = 0; i < logBuffer.Size; ++i) {
		free(logBuffer[i]);
	}
	logBuffer.clear();
}

void GUITerminal::Clear() {
	for (int i = 0; i < logBuffer.Size; ++i) {
		free(logBuffer[i]);
	}
	logBuffer.clear();
}

#ifdef  _DEBUG
void GUITerminal::ClearOpenGLLog() {

	int oldSize = logBuffer.Size;

	for (int i = 0; i < logBuffer.Size; ++i) {
		if (strstr(logBuffer[i], "[OpenGL Debug]"))
			logBuffer.erase(&logBuffer[i]);
	}

	LOG(_INFO, "Deleting OpenGLLog | Before: %d | After: %d", oldSize, logBuffer.Size);

}
#endif

bool PassFilter(const char* line, std::map<std::string, bool>& log_filter) {

	std::string tmp(line);
	size_t pos = tmp.find("]");
	std::string type = tmp.substr(0, pos + 1);
	auto it = log_filter.find(type);
	return !(it != log_filter.end() && !it->second);

}

void GUITerminal::Draw() {

	static std::map<std::string, bool> log_filter = { {_INFO, false}, {_WARNING, false}, {_ERROR, false} };

	std::string wName(ICON_FA_TERMINAL " "); wName.append(name);
	ImGui::Begin(wName.c_str(), &active, ImGuiWindowFlags_NoCollapse);

	ImGui::Columns(2);

	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
	if (logToClipboard) ImGui::LogToClipboard();

	for (int i = 0; i < logBuffer.Size; ++i) {
		const char* line = logBuffer[i];
		if (!filter.PassFilter(line)) continue;

		if (!PassFilter(line, log_filter)) continue;

		ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		if (strstr(line, _ERROR)) color = ImVec4(0.9255f, 0.3451f, 0.3451f, 1.0f);
		else if (strstr(line, _WARNING)) color = ImVec4(1.0000f, 0.8784f, 0.3647f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(line);
		ImGui::PopStyleColor();
	}

	if (scrollToBottom)
		ImGui::SetScrollHere(1.0f);
	scrollToBottom = false;
	if (logToClipboard) {
		ImGui::LogFinish();
		logToClipboard = false;
	}
	ImGui::PopStyleVar();
	ImGui::EndChild();

	ImGui::ColumnSeparator();

	filter.Draw("Search", -100.0f);

	ImGui::NextColumn();
	ImGui::BeginChild("Options");

	ImGui::Text("Actions");
	ImGui::Indent();
	if (ImGui::Button("Clear")) Clear();
	if (ImGui::Button("Copy"))  logToClipboard = true;
	ImGui::Unindent();

	ImGui::ColumnSeparator();
	ImGui::Text("Log type");
	ImGui::Indent();

	for (auto it = log_filter.begin(); it != log_filter.end(); it++) {
		ImGui::Checkbox(it->first.c_str(), &it->second);
	}
	ImGui::Unindent();

	ImGui::EndChild();

	ImGui::End();
}

void GUITerminal::Disable() {
	active = false;
}