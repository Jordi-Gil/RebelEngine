#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include <SDL/SDL.h>

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow();
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	Uint32 GetWindowID() { return SDL_GetWindowID(window); }

public:
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screen_surface = NULL;
};

#endif // __ModuleWindow_H__