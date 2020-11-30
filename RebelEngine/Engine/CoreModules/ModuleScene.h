#pragma once
#include "Module.h"

#include "Main/GameObject.h"

class ModuleScene : public Module {

	ModuleScene(){}

	~ModuleScene(){}

	bool Init();
	update_status Update();

	std::unique_ptr<GameObject> root;

};

