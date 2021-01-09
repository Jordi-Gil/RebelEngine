#include "ModuleScene.h"

#include "Components/ComponentCamera.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"

#include "CoreModules/ModuleResourceManagement.h"

#include "Main/Application.h"

#include "Main/Skybox.h"
#include "Main/GameObject.h"

#include "AccelerationDataStructures/BVH.h"

#include <algorithm>
#include <iostream>
#include <fstream>

ModuleScene::ModuleScene() {

}

bool ModuleScene::Start() {
	return true;
}

ModuleScene::~ModuleScene() {

	delete _goSelected;
	_goSelected = nullptr;

	delete _skybox;
	_skybox = nullptr;

	for (uint i = 0; i < _meshObjects.size(); i++) delete _meshObjects[i];
	_meshObjects.clear();
	_meshObjects.shrink_to_fit();

}

bool ModuleScene::Init() {

	_skybox = new Skybox();

	_root = std::make_unique<GameObject>("Hierarchy");

	std::unique_ptr<ComponentTransform> transform = std::make_unique<ComponentTransform>();
	transform->SetOwner(_root.get());
	_root->AddComponent(std::move(transform));

	std::unique_ptr<GameObject> go = App->resourcemanager->_poolGameObjects.get();
	go->SetName("Camera");
	transform = std::make_unique<ComponentTransform>();
	std::unique_ptr<ComponentCamera> cam = std::make_unique<ComponentCamera>();

	go->SetParent(_root.get());
	transform->SetOwner(go.get());
	cam->SetOwner(go.get());

	go->AddComponent(std::move(transform));
	go->AddComponent(std::move(cam));

	_root->AddChild(std::move(go));

	CreateAABBTree();

	return true;
}

void ModuleScene::DrawRecursive(GameObject &go) {

	for (auto const& children : go.GetChildren()) {
		DrawRecursive(*children);
	}

	if (go.HasComponent(type_component::MESHRENDERER)) {
		ComponentMeshRenderer* comp = static_cast<ComponentMeshRenderer*>(go.GetComponent(type_component::MESHRENDERER));
		comp->Draw();
	}

}

void ModuleScene::Draw() {
	DrawRecursive(*_root);
}

void ModuleScene::InsertOrdered(GameObject &go) {

	auto cutoffCompare = [](const GameObject *a, const GameObject *b) {
		return a->GetMorton() < b->GetMorton();
	};

	auto it = std::upper_bound(_meshObjects.begin(), _meshObjects.end(), &go, cutoffCompare);
	_meshObjects.insert(it, &go);

}

void ModuleScene::CreateAABBTree() {

	

}

bool ModuleScene::Save() 
{
	Json::Value value;
	this->ToJson(value,0);
	Json::StyledWriter wr;
	std::ofstream ofs(DEFAULT_SCENE_PATH DEFAULT_SCENE_NAME DEFAULT_SCENE_EXT);
	std::string st = wr.write(value);
	ofs.write(st.c_str(), st.size());

	return true;
}
bool ModuleScene::ToJson(Json::Value& value, int pos)
{
	Json::Value childrenList;
	_root->ToJson(childrenList, 0);
	value[pos][JSON_TAG_NAME] = DEFAULT_SCENE_NAME;
	value[pos][JSON_TAG_ROOT] = childrenList;

	return true;
}