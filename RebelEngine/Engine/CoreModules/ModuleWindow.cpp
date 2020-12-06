#include "ModuleWindow.h"
#include "ModuleTexture.h"

#include "Utils/Globals.h"

#include "Main/Application.h"

#include <SDL/SDL_image.h>

#include <GL/glew.h>

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
		Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

		if (RESIZABLE) flags |= SDL_WINDOW_RESIZABLE;
		if (FULLSCREEN) flags |= SDL_WINDOW_FULLSCREEN;
		if (BORDERLESS) flags |= SDL_WINDOW_BORDERLESS;
		if (FULLDESKTOP) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

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

	SDL_Surface *icon = IMG_Load("Assets/Images/Logo/rebel_logo_no_text.png");
	SDL_SetWindowIcon(window, icon);

	SDL_MaximizeWindow(window);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // desired version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // we want a double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // we want to have a depth buffer with 24 bits
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // we want to have a stencil buffer with 8 bits

	return ret;
}

bool ModuleWindow::Start() {

	TextureInformation info;
	texIcon = App->texturer->loadTexture("Assets/Images/Logo/rebel_logo.png", info);
	return true;
}

#pragma region setters

void ModuleWindow::SetWindowWidth(int width) {
	currentWidth = width;
	SDL_SetWindowSize(window, currentWidth, currentHeight);
}

void ModuleWindow::SetWindowHeight(int height) {
	currentHeight = height;
	SDL_SetWindowSize(window, currentWidth, currentHeight);
}

void ModuleWindow::SetWindowBrightness(float brightness) {
	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::ResizeWindow(int width, int height) {
	currentWidth = width; currentHeight = height;
}

void ModuleWindow::SetWindowFullScreen(bool fullSc) {
	if (fullSc) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	else SDL_SetWindowFullscreen(window, 0);
}

void ModuleWindow::SetWindowResizable(bool resizable) {
	if(resizable) SDL_SetWindowResizable(window, SDL_TRUE);
	else SDL_SetWindowResizable(window, SDL_FALSE);
}

void ModuleWindow::SetWindowBorderless(bool borderless) {
	if (borderless) SDL_SetWindowBordered(window, SDL_FALSE);
	else SDL_SetWindowBordered(window, SDL_TRUE);
}

void ModuleWindow::SetWindowFullScreenDesktop(bool fulldesktop) {
	if (fulldesktop) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else SDL_SetWindowFullscreen(window, 0);
}

#pragma endregion setters

int ModuleWindow::GetWindowRefreshRate() const {

	int rfr = 0;

	SDL_DisplayMode displayMode;

	if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0) {
		LOG(_ERROR, "SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
	}
	else rfr = displayMode.refresh_rate;

	return rfr;

}

// Called before quitting
bool ModuleWindow::CleanUp() {

	LOG(_INFO, "Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if (window != NULL) {
		SDL_FreeSurface(screen_surface);
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}