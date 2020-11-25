#include "Utils/Globals.h"

#include "Main/Application.h"

#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleEditorCamera.h"
#include "ModuleWindow.h"
#include "ModuleModel.h"

#include "ImGui/imgui_impl_sdl.h"

#include <assimp/Importer.hpp>

#include <SDL/SDL.h>

#define MAX_KEYS 300

ModuleInput::ModuleInput(){
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, (int)KeyState::KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
}

// Destructor
ModuleInput::~ModuleInput(){
	delete[] keyboard;
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

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KeyState::KEY_IDLE)
				keyboard[i] = KeyState::KEY_DOWN;
			else
				keyboard[i] = KeyState::KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KeyState::KEY_REPEAT || keyboard[i] == KeyState::KEY_DOWN)
				keyboard[i] = KeyState::KEY_UP;
			else
				keyboard[i] = KeyState::KEY_IDLE;
		}
	}

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouse_buttons[i] == KeyState::KEY_DOWN)
			mouse_buttons[i] = KeyState::KEY_REPEAT;

		if (mouse_buttons[i] == KeyState::KEY_UP)
			mouse_buttons[i] = KeyState::KEY_IDLE;
	}

    SDL_Event sdlEvent;

	mouseWheel = 0;

    while (SDL_PollEvent(&sdlEvent) != 0)
    {
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

		switch (sdlEvent.type)
		{
			case SDL_QUIT:
				return UPDATE_STOP;
			case SDL_WINDOWEVENT:
				switch (sdlEvent.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
					case SDL_WINDOWEVENT_SIZE_CHANGED:
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

			case SDL_DROPFILE: {      // In case if dropped file
				char* dropped_filedir = sdlEvent.drop.file;
				LOG(_INFO, "Dropped file: %s", dropped_filedir);
				App->models->LoadModel(dropped_filedir);
				SDL_free(dropped_filedir);
				break;
			}
		}
    }

    return UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp() {
	LOG(_INFO, "Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}
