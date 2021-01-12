#include "Utils/Globals.h"

#include "Main/Application.h"

#include "ModuleEditorCamera.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleModel.h"
#include "ModuleScene.h"
#include "ImGui/imgui_impl_sdl.h"

#include <assimp/Importer.hpp>

#include <SDL/SDL.h>

#define MAX_KEYS 300

constexpr char* imageFormat[] = { 
	".blp", ".bmp", ".bw", ".cur", ".cut", ".dcm", ".dcx", ".dds", ".dicom", ".dpx", ".exr", ".fit", 
	".fits", ".ftx", ".gif", ".h", ".hdp", ".hdr", ".icns", ".ico", ".iff", ".im", ".iwi", ".jng", ".jp2", 
	".jpe", ".jpeg", ".jpg", ".lbm", ".lif", ".lmp", ".mdl", ".mng", ".mp3", ".pbm", ".pcd", ".pcx", ".pgm", 
	".pic", ".pix", ".png", ".pnm", ".ppm", ".psd", ".psp", ".pxr", ".ras", ".rgb", ".rgba", ".rot", ".rs", ".sgi", 
	".sun", ".texture", ".tga", ".tif", ".tiff", ".tpl", ".utx", ".vtf", ".wal", ".wdp", ".xpm" };

constexpr char* meshFormat[] = { ".3d", ".3ds", ".ac", ".ase", ".b3d", ".blend", ".bvh", ".cob", ".csm",
".dae", ".dxf", ".fbx", ".glb", ".gltf", ".hmp", ".ifc", ".irr", ".irrmesh",
".lwo", ".lws", ".lxo", ".md2", ".md3", ".md5", ".mdc", ".mdl", ".mdl", ".ms3d",
".ndo", ".nff", ".nff", ".obj", ".off", ".ogex", ".pk3", ".ply", ".q3d", ".q3s",
".raw", ".scn", ".smd", ".stl", ".ter", ".vta", ".x", ".xgl", ".xml", ".zgl" };

char asciitolower(char in) {
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}

bool ImageSupported(const char *path) {

	char extension[_MAX_EXT];
	errno_t error = _splitpath_s(path, NULL, 0, NULL, 0, NULL, 0, extension, _MAX_EXT);
	if (error != 0) { 
		char errmsg[256];
		strerror_s(errmsg, 256, error);
		LOG(_ERROR, "Couldn't split the given path %s. Error: %s", path, errmsg);
		return false; 
	}

	std::string ext(extension);
	std::transform(ext.begin(), ext.end(), ext.begin(), asciitolower);

	for (int i = 0; i < MARRAYSIZE(imageFormat); i++)
		if (std::strcmp(imageFormat[i], ext.c_str()) == 0) return true;
	return false;
}

bool MeshSupported(const char* path) {

	char extension[_MAX_EXT];
	errno_t error = _splitpath_s(path, NULL, 0, NULL, 0, NULL, 0, extension, _MAX_EXT);
	if (error != 0) {
		char errmsg[256];
		strerror_s(errmsg, 256, error);
		LOG(_ERROR, "Couldn't split the given path %s. Error: %s", path, errmsg);
		return false;
	}

	std::string ext(extension);
	std::transform(ext.begin(), ext.end(), ext.begin(), asciitolower);

	for (int i = 0; i < MARRAYSIZE(meshFormat); i++)
		if (std::strcmp(meshFormat[i], ext.c_str()) == 0) return true;
	return false;
}

ModuleInput::ModuleInput(){
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, (int)KeyState::KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
}

// Destructor
ModuleInput::~ModuleInput(){
	delete[] keyboard;
	keyboard = nullptr;
}

// Called before render is available
bool ModuleInput::Init() {

	LOG(_INFO, "Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0) {
		LOG(_ERROR, "SDL_EVENTS could not initialize! SDL_Error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::Update() {

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i) {

		if (keys[i] == 1) {
			if (keyboard[i] == KeyState::KEY_IDLE)
				keyboard[i] = KeyState::KEY_DOWN;
			else
				keyboard[i] = KeyState::KEY_REPEAT;
		}
		else {
			if (keyboard[i] == KeyState::KEY_REPEAT || keyboard[i] == KeyState::KEY_DOWN)
				keyboard[i] = KeyState::KEY_UP;
			else
				keyboard[i] = KeyState::KEY_IDLE;
		}
	}

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i) {

		if (mouse_buttons[i] == KeyState::KEY_DOWN)
			mouse_buttons[i] = KeyState::KEY_REPEAT;

		if (mouse_buttons[i] == KeyState::KEY_UP)
			mouse_buttons[i] = KeyState::KEY_IDLE;
	}

    SDL_Event sdlEvent;

	mouseWheel = 0;

    while (SDL_PollEvent(&sdlEvent) != 0) {

		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
		
		switch (sdlEvent.type) {

			case SDL_QUIT:
				App->scene->Save();
				return UPDATE_STOP;
			case SDL_WINDOWEVENT:
				switch (sdlEvent.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						if (sdlEvent.window.windowID == App->window->GetWindowID()) 
							App->window->ResizeWindow(sdlEvent.window.data1, sdlEvent.window.data2);
						break;
					case SDL_WINDOWEVENT_CLOSE:
						return UPDATE_STOP;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[sdlEvent.button.button - 1] = KeyState::KEY_DOWN;
				break;
			case SDL_MOUSEBUTTONUP:
				mouse_buttons[sdlEvent.button.button - 1] = KeyState::KEY_UP;
				break;
			case SDL_MOUSEWHEEL:
				mouseWheel = sdlEvent.wheel.y;
				break;

			case SDL_DROPFILE:
				char* dropped_filedir = sdlEvent.drop.file;
				LOG(_INFO, "Dropped file: %s", dropped_filedir);
				if (ImageSupported(dropped_filedir)) App->models->LoadTexture(dropped_filedir);
				else if (MeshSupported(dropped_filedir)) App->models->LoadModel(dropped_filedir);
				else LOG(_ERROR, "The file dropped has not a valid extension for mesh/texture loader.");
				SDL_free(dropped_filedir);
				break;
		}
    }

    return UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp() {

	LOG(_INFO, "Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}
