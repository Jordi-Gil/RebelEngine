#pragma once

#include "Module.h"

#include "Math/float2.h"

#include "Utils/Globals.h"

#define NUM_MOUSE_BUTTONS 5

enum class KeyState {

	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module {

public:
	
	ModuleInput();
	~ModuleInput();

	bool Init() override;
	bool CleanUp() override;
	update_status PreUpdate() override;

	KeyState GetKey(int id) const { return keyboard[id]; }
	KeyState GetMouseButtonDown(int id) const { return mouse_buttons[id - 1]; }

	int GetMouseWheel() const { return mouseWheel; }

	const float2& GetMouseMotion() const { return mouse_motion; }
	const float2& GetMousePosition() const { return mouse; }

private:

	KeyState* keyboard;
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS];
	int mouseWheel;
	float2 mouse_motion = { 0, 0 };
	float2 mouse = { 0, 0 };
};