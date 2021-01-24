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

	void DrawHierarchy(GameObject& go);

	uint _dragged;
	uint _popuped;
	GameObject* _go_dragged;
	
};