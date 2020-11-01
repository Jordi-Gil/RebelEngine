#pragma once
#include <SDL/SDL.h>

class Timer
{

public:
	Timer() : startTime(0), stopTime(0), isStopped(true) {};

	void start() {
		isStopped = false;
		startTime = SDL_GetTicks();
	}
	void stop() {
		isStopped = true;
		startTime = SDL_GetTicks();
	}

	Uint32 getDeltaTime() {
		if (isStopped) {
			return stopTime - startTime;
		}
		else return SDL_GetTicks() - startTime;
	}
	
private:

	Uint32 startTime;
	Uint32 stopTime;

	bool isStopped;

};

