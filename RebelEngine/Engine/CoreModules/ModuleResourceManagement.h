#pragma once
#include "Module.h"

#include "Main/Pool.h"

#include "Main/GameObject.h"

class ModuleResourceManagement : public Module {

public:

	ModuleResourceManagement();

	bool Init() override;
	void Load3DModel(const char* path);

public:

	Pool<GameObject> _gameObjects;
	Pool<Component> _components;

};


