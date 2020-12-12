#pragma once
#include "CoreModules/ModuleGUI.h"

class GameObject;

class GUIHierarchy : public ModuleGUI {

public:

	GUIHierarchy(const char* _name);

	void Draw() override;

	bool IsActive() const override { return active; };
	const char* GetName() const { return name; }

	void ToggleActive() override;

private:

	void DrawHierarchy(GameObject& go, unsigned int depth);

	int dragged;
	unsigned int dragged_depth;
	GameObject* go_dragged;
};