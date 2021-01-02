#include "ModuleResourceManagement.h"
#include "ModuleModel.h"

#include "Main/Application.h"

ModuleResourceManagement::ModuleResourceManagement() {
}

bool ModuleResourceManagement::Init() {

	_poolGameObjects = Pool<GameObject>(1000);
	return true;
}

void ModuleResourceManagement::Load3DModel(const char *path) {

	//Load into custom JSON file
	App->models->LoadModel(path);
}

void ModuleResourceManagement::AddGameObject(std::unique_ptr<GameObject>&& go) {
	_gameObjects.push_back(std::move(go));
}