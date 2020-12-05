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
	
	update_status PreUpdate() override;

	void Draw();

private:

	void DrawRecursive(GameObject &go);

public:

	std::unique_ptr<GameObject> root;
	GameObject* goSelected = nullptr;

	

	bool frustum = false;

};

