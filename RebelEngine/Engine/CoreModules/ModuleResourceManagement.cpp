#include "ModuleResourceManagement.h"
#include "ModuleModel.h"

#include "Main/Application.h"

ModuleResourceManagement::ModuleResourceManagement() {
}

bool ModuleResourceManagement::Init() {

	_gameObjects = Pool<GameObject>(1000);
	return true;
}

void ModuleResourceManagement::Load3DModel(const char *path) {

	//Load into custom JSON file
	App->models->LoadModel(path);
}
