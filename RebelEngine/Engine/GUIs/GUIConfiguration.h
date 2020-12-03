#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIConfiguration : public ModuleGUI {

public:

	GUIConfiguration(const char* _name);

	void Draw() override;

	bool IsActive() override { return active; };

	const char* GetName() { return name; }

	void ToggleActive();

	void AddFPS(float FPS, float ms);

private:

	std::vector<float> fpsHist;
	std::vector<float> msHist;

};

