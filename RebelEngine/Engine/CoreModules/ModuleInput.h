#pragma once
#include "Module.h"
#include "Utils/Globals.h"

#define NUM_MOUSE_BUTTONS 5

enum class KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput();

	bool Init();
	update_status Update();
	bool CleanUp();

	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

	int GetMouseWheel() {
		return mouseWheel;
	}

private:

	KeyState* keyboard;
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS];
	int mouseWheel;
};