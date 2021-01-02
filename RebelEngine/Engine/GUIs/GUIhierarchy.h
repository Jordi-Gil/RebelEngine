#pragma once
#include "CoreModules/ModuleGUI.h"

class GameObject;

class GUIHierarchy : public ModuleGUI {

public:

	GUIHierarchy(const char* name);

	void Draw() override;

	bool IsActive() const override { return _active; };
	const char* GetName() const { return _name; }

	void ToggleActive() override;

private:

	void DrawHierarchy(GameObject& go, uint depth);

	uint _dragged;
	uint _dragged_depth;
	GameObject* _go_dragged;
};