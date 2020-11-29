#pragma once
#include <SDL/SDL.h>

class MSTimer
{

public:
	MSTimer() : startTime(0), stopTime(0), isStopped(true) {};

	void start() {
		isStopped = false;
		startTime = SDL_GetTicks();
	}

	void stop() {
		isStopped = true;
		stopTime = SDL_GetTicks();
	}

	Uint32 read() {
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

class uSTimer {

	void start() {
		isStopped = true;
		startTime = SDL_GetPerformanceCounter();
	}

	void stop() {
		isStopped = true;
		stopTime = SDL_GetPerformanceCounter();
	}

	Uint64 read() {
		if (isStopped) {
			return (stopTime - startTime) / SDL_GetPerformanceFrequency();
		}
		else return (SDL_GetPerformanceCounter() - startTime) / SDL_GetPerformanceFrequency();
	}

	Uint64 startTime;
	Uint64 stopTime;

	bool isStopped = false;


};