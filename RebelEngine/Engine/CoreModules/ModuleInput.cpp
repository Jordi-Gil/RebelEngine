#include "ModuleInput.h"

#include "Utils/Globals.h"

#include "Main/Application.h"

#include "ModuleEditorCamera.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleModel.h"
#include "ModuleScene.h"
#include "ImGui/imgui_impl_sdl.h"

#include <assimp/Importer.hpp>

#include <SDL/SDL.h>
#include <string>

#define MAX_KEYS 300

bool ImageSupported(const char *path) {

	char extension[_MAX_EXT];
	_splitpath_s(path, NULL, 0, NULL, 0, NULL, 0, extension, _MAX_EXT);

	for (int i = 0; i < MARRAYSIZE(imageFormat); i++)
		if (_strcmpi(imageFormat[i], extension) == 0) return true;
	return false;
}

bool MeshSupported(const char* path) {

	char extension[_MAX_EXT];
	_splitpath_s(path, NULL, 0, NULL, 0, NULL, 0, extension, _MAX_EXT);
	
	for (int i = 0; i < MARRAYSIZE(meshFormat); i++)
		if (_strcmpi(meshFormat[i], extension) == 0) return true;
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
update_status ModuleInput::PreUpdate() {

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
	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	mouse.x = mouse_x;
	mouse.y = mouse_y;
	mouse_motion.x = 0;
	mouse_motion.y = 0;
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
						App->scene->Save();
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
			case SDL_MOUSEMOTION:
				mouse_motion.x += sdlEvent.motion.xrel;
				mouse_motion.y += sdlEvent.motion.yrel;
				break;
			case SDL_DROPFILE:
				char* dropped_filedir = sdlEvent.drop.file;
				LOG(_INFO, "Dropped file: %s", dropped_filedir);
				if (ImageSupported(dropped_filedir)) break;//App->models->LoadTexture(dropped_filedir);
				else if (MeshSupported(dropped_filedir)) App->models->LoadModelFromFBX(dropped_filedir);
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
