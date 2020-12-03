#pragma once

#include "Module.h"
#include <SDL/SDL.h>

class Application;

class ModuleWindow : public Module {

public:

	ModuleWindow();
	~ModuleWindow();

	bool Start() override;

	bool Init() override;
	bool CleanUp() override;

	Uint32 GetWindowID() { return SDL_GetWindowID(window); }
	int GetCurrentWidth() { return currentWidth; }
	int GetCurrentHeight() { return currentHeight; }
	float GetWindowBrightness() { return SDL_GetWindowBrightness(window); }
	float GetWindowRefreshRate();

	void SetWindowWidth(int width);
	void SetWindowHeight(int height);
	void SetWindowBrightness(float brightness);
	void SetWindowFullScreen(bool fullSc);
	void SetWindowResizable(bool resizable);
	void SetWindowBorderless(bool borderless);
	void SetWindowFullScreenDesktop(bool fulldesktop);

	void ResizeWindow(int width, int height);

public:

	SDL_Window* window = NULL;
	SDL_Surface* screen_surface = NULL;

	int minWidth = 1050, minHeight = 250;
	int maxWidth, maxHeight;

	unsigned int texIcon = 0;

private:

	int currentWidth, currentHeight;
};