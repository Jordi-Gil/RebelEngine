#pragma once

#include "CoreModules/ModuleGUI.h"

class GUIConfiguration : public ModuleGUI {

public:

	GUIConfiguration(const char* name);

	void Draw() override;

	bool IsActive() const override { return _active; };

	const char* GetName() const override { return _name; }

	void ToggleActive() override;

	void AddFPS(float FPS, float ms);

private:

	std::vector<float> _fpsHist;
	std::vector<float> _msHist;

};

