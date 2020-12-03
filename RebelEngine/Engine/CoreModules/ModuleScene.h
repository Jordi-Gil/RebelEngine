#pragma once
#include "Module.h"

#include "Main/GameObject.h"

class ModuleScene : public Module {

public:
	ModuleScene() { root = std::make_unique<GameObject>();
	}

	~ModuleScene(){}

	bool Init() override;
	update_status Update() override;

public:
	std::unique_ptr<GameObject> root;

};

