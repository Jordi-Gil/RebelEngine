#pragma once
#include "Module.h"

#include "Main/Pool.h"

#include "Main/GameObject.h"

class ModuleResourceManagement : public Module {

public:

	ModuleResourceManagement();

	bool Init() override;
	void Load3DModel(const char* path);
	void AddGameObject(std::unique_ptr<GameObject>&& go);

public:

	Pool<GameObject> _poolGameObjects;
	std::vector<std::unique_ptr<GameObject>> _gameObjects;

};


