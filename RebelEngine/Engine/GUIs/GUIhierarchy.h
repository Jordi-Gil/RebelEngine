#pragma once
#include "CoreModules/ModuleGUI.h"

class GUIHierarchy : public ModuleGUI {

public:

	GUIHierarchy(const char* _name);

	void Draw() override;

	bool IsActive() const override { return active; };
	const char* GetName() const { return name; }

	void ToggleActive() override;
};
