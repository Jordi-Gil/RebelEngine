#pragma once
#include "Module.h"

#include "Main/Pool.h"

#include "Main/GameObject.h"

class ModuleResourceManagement : public Module {

	ModuleResourceManagement();

	bool Init() override;

private:

	Pool<GameObject> *gameObjects;
	//Pool<Materials>
	//Pool<Textrues>
	//...

};

