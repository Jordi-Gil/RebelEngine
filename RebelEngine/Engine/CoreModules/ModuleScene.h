#pragma once
#include "Module.h"

#include "Main/Pool.h"

#include <memory>
#include <vector>

class Octree;
class OctreeNode;

class Skybox;
class GameObject;
class ComponentCamera;

enum Rebel_FrustumMask {
	NO_FRUSTUM  = 1,
	LINEAR_AABB = 1 << 1,
	OCTREE		= 1 << 2
};

class ModuleScene : public Module {

public:

	ModuleScene();
	~ModuleScene();

	bool Init() override;
	bool Start() override;

	update_status PreUpdate() override;

	void Draw();

	void SetMask(Rebel_FrustumMask mask) { _mask = mask; }

private:

	void IterateRoot(GameObject &go);
	void DrawRecursive(GameObject &go);
	void DrawFrustumOutput();

	void FrustumCulling(OctreeNode* node);
	void FrustumCulling();

public:

	Pool<GameObject> _poolGameObjects;
	std::unique_ptr<GameObject> _root;
	Skybox* _skybox;

private:

	bool _frustum = false;

	int _mask = NO_FRUSTUM;

	GameObject* _goSelected = nullptr;
	ComponentCamera* _mainCamera = nullptr;

	Octree* _octree = nullptr;

	std::vector<GameObject*> _objects;
	std::vector<GameObject *> _objectsToDraw;

};

