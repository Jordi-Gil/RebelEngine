#include "ModuleScene.h"

#include "Components/ComponentCamera.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"

#include "Main/Application.h"

#include "Main/Skybox.h"
#include "Main/GameObject.h"

#include "AccelerationDataStructures/BVH.h"

#include <algorithm>

ModuleScene::ModuleScene() {

}

ModuleScene::~ModuleScene() {

	if (_bvh) {
		delete _bvh;
		_bvh = nullptr;
	}

	delete _goSelected;
	_goSelected = nullptr;

	delete _skybox;
	_skybox = nullptr;

	//for (uint i = 0; i < _meshObjects.size(); i++) delete _meshObjects[i];
	_meshObjects.clear();
	_meshObjects.shrink_to_fit();

}

bool ModuleScene::Init() {

	_poolGameObjects = Pool<GameObject>(1000);

	_skybox = new Skybox();

	_root = std::make_unique<GameObject>("Hierarchy");
	_root->AddMask(GO_MASK_ROOT_NODE);

	std::unique_ptr<ComponentTransform> transform = std::make_unique<ComponentTransform>();
	transform->SetOwner(_root.get());
	_root->AddComponent(std::move(transform));

	std::unique_ptr<GameObject> go = _poolGameObjects.get();
	go->SetName("Camera");
	transform = std::make_unique<ComponentTransform>();
	std::unique_ptr<ComponentCamera> cam = std::make_unique<ComponentCamera>();

	go->SetParent(_root.get());
	transform->SetOwner(go.get());
	cam->SetOwner(go.get());

	cam->SetPosition(transform->GetPosition());

	go->AddComponent(std::move(transform));
	go->AddComponent(std::move(cam), GO_MASK_CAMERA);

	_root->AddChild(std::move(go));

	return true;
}

void ModuleScene::IterateRoot(GameObject& go) {

	for (auto const& children : go.GetChildren()) {
		IterateRoot(*children);
	}

	//Insert in vector to construct BVH/Quadtree/Octree
	if (go.HasMesh()) InsertOrdered(go);
	//else if( (go.GetMask() & GO_MASK_ROOT_NODE) == 0 ) _GameObjectsToDraw.push_back(&go);

}

bool ModuleScene::Start() {

	IterateRoot(*_root);
	if(_meshObjects.size() > 0) _bvh = new BVH(_meshObjects);

#ifdef  _DEBUG
	if (_meshObjects.size() > 0) {
		LOG(_ERROR, "%s", " ");
		LOG(_ERROR, "%s", " ");
		_bvh->Print();
		LOG(_ERROR, "%s", " ");
		LOG(_ERROR, "%s", " ");
	}
#endif //  _DEBUG

	return true;
}

update_status ModuleScene::PreUpdate() {


	

	return UPDATE_CONTINUE;
}

void ModuleScene::DrawRecursive(GameObject &go) {

	for (auto const& children : go.GetChildren()) {
		DrawRecursive(*children);
	}

	for (auto const& component : go.GetComponents()) {
		component->Draw();
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
