#pragma once
#include <windows.h>
#include <stdio.h>

#define LOG(type, format,  ...) log(__FILE__, __LINE__, type, format, __VA_ARGS__);

void log(const char file[], int line, const char* type, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define RESIZABLE true
#define FULLSCREEN false
#define FULLDESKTOP false
#define BORDERLESS false

#define VSYNC true
#define TITLE "Rebel Engine"
#define ORGANIZATION "Rebel Technologies"
#define VERSION "1.0.0"

#define _ERROR "[error]"
#define _INFO "[info]"
#define _WARNING "[warning]"