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

	Uint32 GetWindowID() const { return SDL_GetWindowID(window); }
	int GetCurrentWidth() const { return currentWidth; }
	int GetCurrentHeight() const { return currentHeight; }
	float GetWindowBrightness() const { return SDL_GetWindowBrightness(window); }
	int GetWindowRefreshRate() const;

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