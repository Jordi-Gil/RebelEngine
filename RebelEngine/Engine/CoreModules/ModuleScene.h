#pragma once
#include "Module.h"

#include "Main/Pool.h"

#include <memory>
#include <vector>

class BVH;
class Skybox;
class GameObject;

class ModuleScene : public Module {

public:

	ModuleScene();
	~ModuleScene();

	bool Init() override;
	bool Start() override;

	update_status PreUpdate() override;

	void Draw();

	void InsertOrdered(GameObject& go);

private:

	void IterateRoot(GameObject &go);
	void DrawRecursive(GameObject &go);

public:

	Pool<GameObject> _poolGameObjects;
	std::unique_ptr<GameObject> _root;
	Skybox* _skybox;

private:

	GameObject* _goSelected = nullptr;
	std::vector<GameObject *> _meshObjects;
	std::vector<GameObject *> _GameObjectsToDraw;

	bool _frustum = false;

	BVH* _bvh = nullptr;

};

