#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIConfiguration : public ModuleGUI {

public:

	GUIConfiguration(const char* _name);

	void Draw() override;

	bool IsActive() const override { return active; };

	const char* GetName() const override { return name; }

	void ToggleActive() override;

	void AddFPS(float FPS, float ms);

private:

	std::vector<float> fpsHist;
	std::vector<float> msHist;

};

