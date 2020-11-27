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

static const Uint64 frequency = SDL_GetPerformanceFrequency();

class uSTimer {

	void start() {
		isStopped = true;
		startTime = SDL_GetPerformanceCounter();
	}

	void stop() {
		isStopped = true;
		stopTime = SDL_GetPerformanceCounter();
	}

	double read() {
		if (isStopped) {
			return (stopTime - startTime) / (double) frequency;
		}
		else return (SDL_GetPerformanceCounter() - startTime) / (double)frequency;
	}

	Uint64 startTime;
	Uint64 stopTime;

	bool isStopped = false;


};