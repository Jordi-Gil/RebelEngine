#pragma once
#include "Module.h"

#include "Main/GameObject.h"

class ModuleScene : public Module {

public:

	ModuleScene();

	~ModuleScene(){
		delete goSelected;
		goSelected = nullptr;
	}

	bool Init() override;
	update_status Update() override;

public:

	std::unique_ptr<GameObject> root;
	GameObject* goSelected = nullptr;

};

