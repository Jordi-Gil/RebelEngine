#include "Utils/Globals.h"

#include "Main/Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow() { }

// Destructor
ModuleWindow::~ModuleWindow() { }

// Called before render is available
bool ModuleWindow::Init() {

	LOG(_INFO, "Init SDL window & surface");
	bool ret = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {

		LOG(_ERROR, "SDL_VIDEO could not initialize! SDL_Error: %s", SDL_GetError());
		ret = false;
	}
	else {

		//Create window
		int width = SCREEN_WIDTH;
		int height = SCREEN_HEIGHT;
		Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

		if (FULLSCREEN) flags |= SDL_WINDOW_FULLSCREEN;

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
		currentWidth = width; currentHeight = height;

		if (window == NULL) {

			LOG(_ERROR, "Window could not be created! SDL_Error: %s", SDL_GetError());
			ret = false;
		}
		else {

			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}

		SDL_Rect r;
		if (SDL_GetDisplayBounds(0, &r) != 0) {
			LOG(_ERROR, "SDL_GetDisplayBounds failed: %s", SDL_GetError());
			ret = false;
		}

		maxWidth = r.w; maxHeight = r.h;

	}

	SDL_SetWindowMinimumSize(window, minWidth, minHeight);
	SDL_SetWindowMaximumSize(window, maxWidth, maxHeight);
	SDL_SetWindowBrightness(window, 1.0f);

	return ret;
}

void ModuleWindow::SetWindowWidth(int width) {
	currentWidth = width;
	SDL_SetWindowSize(window, width, currentHeight);
}

void ModuleWindow::SetWindowHeight(int height) {
	currentHeight = height;
	SDL_SetWindowSize(window, currentWidth, height);
}

void ModuleWindow::SetWindowBrightness(float brightness) {
	SDL_SetWindowBrightness(window, brightness);
}

// Called before quitting
bool ModuleWindow::CleanUp() {

	LOG(_INFO, "Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if (window != NULL) {
		SDL_DestroyWindow(window);
		SDL_FreeSurface(screen_surface);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}