#include "ModuleScene.h"


ModuleScene::ModuleScene() {	
	root = std::make_unique<GameObject>();
}

bool ModuleScene::Init() {
	return true;
}

update_status ModuleScene::Update() {

	return UPDATE_CONTINUE;
}
