#pragma once
#include "Module.h"

#include <memory>
#include <vector>

#include "json/json.h"

class BVH;
class Skybox;
class GameObject;

class ModuleScene : public Module {

public:

	ModuleScene();
	ModuleScene(const Json::Value& value);
	~ModuleScene();

	bool Init() override;
	bool Start() override;

	void Draw();

	void InsertOrdered(GameObject& go);
	bool Save();
	bool Load();
	bool ToJson(Json::Value& value, int pos);
	bool FromJson(const Json::Value& value);
private:

	void DrawRecursive(GameObject &go);
	void CreateAABBTree();

public:

	std::unique_ptr<GameObject> _root;
	Skybox* _skybox;

private:

	GameObject* _goSelected = nullptr;
	//TODO: Move pool here
	std::vector<GameObject *> _meshObjects;

	bool _frustum = false;

	BVH* _BVH;

};

