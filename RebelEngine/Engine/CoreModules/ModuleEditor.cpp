#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleEditorCamera.h"

#include "Main/Application.h"

#include "Utils/Console.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_utils.h"

ModuleEditor::ModuleEditor() {

	FILE* pTextFile;

	pTextFile = fopen("Windows/libraries.txt", "r");

	if (pTextFile) {

		fseek(pTextFile, 0, SEEK_END);
		libSize = ftell(pTextFile);
		libraries_info = (char*)malloc(sizeof(char) * libSize + 1);
		fseek(pTextFile, 0, SEEK_SET);
		fread(libraries_info, 1, libSize, pTextFile);
		libraries_info[libSize] ='\0';
		fclose(pTextFile);
	}
}

ModuleEditor::~ModuleEditor() {

	delete[] libraries_info;

}

bool ModuleEditor::Init() {

	LOG(_INFO, "Init WindowEditor");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.WantCaptureMouse = true;
	io.WantCaptureKeyboard = false;

	PhotosopLikeStyle();

	ImGui::GetIO().Fonts->AddFontDefault();
	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	ImGui::GetIO().Fonts->AddFontFromFileTTF("Fonts/" FONT_ICON_FILE_NAME_FAS, 12.0f, &icons_config, icons_ranges);

	return true;
}

bool ModuleEditor::Start() {

	const char* glsl_version = "#version 460";
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->GetContext());
	ImGui_ImplOpenGL3_Init(glsl_version);

	return true;
}

update_status ModuleEditor::Update() {

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y));
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpaceWnd", 0, window_flags);
	ImGui::PopStyleVar(3);

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	}

	update_status ret = UPDATE_CONTINUE;
	ret = DrawMainMenu();

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	return ret;
}

#pragma region Windows

void ModuleEditor::DrawSceneEditor(bool *show_scene_editor) {

	static float sceneWindowWidth = ImGui::GetWindowWidth();
	static float sceneWindowHeight = ImGui::GetWindowHeight();

	App->renderer->Draw(sceneWindowWidth, sceneWindowHeight);

	ImGui::Begin(ICON_FA_BORDER_ALL " Scene", show_scene_editor, ImGuiWindowFlags_NoCollapse);
	ImVec2 pos = ImGui::GetCursorScreenPos();

	sceneFocused = ImGui::IsWindowFocused();

	ImGui::GetWindowDrawList()->AddImage(
		(void*)(intptr_t)App->renderer->GetViewportTexture(),
		ImVec2(ImGui::GetCursorScreenPos()),
		ImVec2(ImGui::GetCursorScreenPos().x + sceneWindowWidth,
			ImGui::GetCursorScreenPos().y + sceneWindowHeight),
		ImVec2(0, 1),
		ImVec2(1, 0));


	if (sceneWindowWidth != ImGui::GetWindowWidth() || sceneWindowHeight != ImGui::GetWindowHeight())
	{
		sceneWindowWidth = ImGui::GetWindowWidth();
		sceneWindowHeight = ImGui::GetWindowHeight();
		App->editorCamera->WindowResized(sceneWindowWidth, sceneWindowHeight);
	}

	ImGui::End();
}

update_status ModuleEditor::DrawMainMenu() {

	update_status ret = UPDATE_CONTINUE;

	static bool show_about = false;
	static bool show_console = false;
	static bool show_config = false;
	static bool show_scene_editor = true;

	if (show_about) DrawAbout(&show_about);
	if (show_console) console->Draw(ICON_FA_TERMINAL "Console", &show_console);
	if (show_config) DrawConfiguration(&show_config);
	if (show_scene_editor) DrawSceneEditor(&show_scene_editor);

	if (ImGui::BeginMainMenuBar()){

		if (ImGui::BeginMenu("File")) {
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "ALT+F4")) {
				ret = UPDATE_STOP;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows")) {
			ImGui::MenuItem("Scene", NULL, &show_scene_editor, &show_scene_editor);
			ImGui::MenuItem("Console", NULL, &show_console, &show_console);
			ImGui::MenuItem("Configuration", NULL, &show_config, &show_config);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("Documentation")) App->RequestBrowser("https://github.com/Jordi-Gil/RebelEngine/wiki");
			if (ImGui::MenuItem("Download latest")) App->RequestBrowser("https://github.com/Jordi-Gil/RebelEngine/releases");
			if (ImGui::MenuItem("Report a bug")) App->RequestBrowser("https://github.com/Jordi-Gil/RebelEngine/issues");
			ImGui::Separator();
			ImGui::MenuItem("About Rebel", NULL, &show_about);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	return ret;
}

void ModuleEditor::DrawAbout(bool* show_about) {

	ImGui::OpenPopup("About Rebel");

	if (ImGui::BeginPopupModal("About Rebel", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove)) {

		ImGui::SetWindowSize(ImVec2(0, 0));

		ImGui::Text("Version: Rebel Engine"); ImGui::SameLine(); ImGui::Text(VERSION);
		ImGui::Text("Rebel is a game engine programmed entirely in C++.");
		ImGui::Text("Author:"); ImGui::SameLine(); ImGui::Text("Jordi Gil Gonzalez");
		if (ImGui::CollapsingHeader("Libraries used")) {

			char* info = (char*)malloc(sizeof(char) * libSize + 1);
			strcpy(info, libraries_info);

			char* _LIB, * _URL, * _DESC;
			char* token = strtok(info, "\n");
			while (token != NULL) {
				_LIB = token;
				token = strtok(NULL, "\n"); _URL = token;
				token = strtok(NULL, "\n"); _DESC = token;
				token = strtok(NULL, "\n");
				ImGui::BulletText(""); ImGui::SameLine();  ImGui::TextURL(_LIB, _URL, 0); ImGui::SameLine(); ImGui::Text(_DESC);
			}

			free(info);
		}
		ImGui::Text("LICENSE: GNU General Public License v3.0");
		ImGui::NewLine();
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 30);
		if (ImGui::Button("Ok")) {
			ImGui::CloseCurrentPopup();
			*show_about = false;
		}

		ImGui::EndPopup();
	}
}

void ModuleEditor::DrawConfiguration(bool* show_config) {

	ImGui::Begin(ICON_FA_COGS " Configuration", show_config, ImGuiWindowFlags_NoCollapse);

	static int width = App->window->GetCurrentWidth();
	static int height = App->window->GetCurrentHeight();
	static float brightness = App->window->GetWindowBrightness();
	static bool fullscreen = FULLSCREEN;
	static bool resizable = RESIZABLE;

	if (ImGui::CollapsingHeader("Renderer")) {
		
	}

	if (ImGui::CollapsingHeader("Window")) {
		if (ImGui::SliderFloat("Brightness", &brightness, 0.2f, 1.0f)) App->window->SetWindowBrightness(brightness);
		if (ImGui::SliderInt("Width", &width, App->window->minWidth, App->window->maxWidth)) App->window->SetWindowWidth(width);
		if (ImGui::SliderInt("Height", &height, App->window->minHeight, App->window->maxHeight)) App->window->SetWindowHeight(height);
	}

	ImGui::End();
}

#pragma endregion Windows

void ModuleEditor::PhotosopLikeStyle() {

	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
	colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
	colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_DockingPreview] = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_UrlText] = ImVec4(0.000f, 0.451f, 1.0f, 1.0f);

	style->ChildRounding = 4.0f;
	style->FrameBorderSize = 1.0f;
	style->FrameRounding = 2.0f;
	style->GrabMinSize = 7.0f;
	style->PopupRounding = 2.0f;
	style->ScrollbarRounding = 12.0f;
	style->ScrollbarSize = 13.0f;
	style->TabBorderSize = 1.0f;
	style->TabRounding = 0.0f;
	style->WindowRounding = 0.0f;
}

bool ModuleEditor::CleanUp() {

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}