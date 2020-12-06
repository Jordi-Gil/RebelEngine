#pragma once
#include "Module.h"

#include <memory>

class Skybox;
class GameObject;

class ModuleScene : public Module {

public:

	ModuleScene();
	~ModuleScene();

	bool Init() override;
	
	update_status PreUpdate() override;

	void Draw();

private:

	void DrawRecursive(GameObject &go);

public:

	std::unique_ptr<GameObject> root;
	GameObject* goSelected = nullptr;

	Skybox* skybox;

	bool frustum = false;

};

