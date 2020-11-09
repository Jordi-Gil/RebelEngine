#pragma once

#include "Module.h"

class ModuleEditor : public Module
{
public:

	ModuleEditor();

	bool Init();

	update_status Update();

	bool CleanUp();

private:

	void PhotosopLikeStyle();

	update_status DrawMainMenu();
	void DrawAbout(bool& show_about);

};