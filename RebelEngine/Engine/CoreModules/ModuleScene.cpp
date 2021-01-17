#include "ModuleScene.h"

#include "Components/ComponentLight.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"

#include "AccelerationDataStructures/BVH.h"
#include "AccelerationDataStructures/Octree.h"
#include "CoreModules/ModuleEditorCamera.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Materials/Skybox.h"

#include <algorithm>
#include <iostream>
#include <fstream>

ModuleScene::ModuleScene() {

}

ModuleScene::ModuleScene(const Json::Value& value) {
	FromJson(value);
}

ModuleScene::~ModuleScene() {

	delete _goSelected;
	_goSelected = nullptr;

	delete _skybox;
	_skybox = nullptr;

	delete _octree;
	_octree = nullptr;

}

bool ModuleScene::Init() {

	_poolGameObjects = Pool<GameObject>(1000);

	_skybox = new Skybox();
	
	/*
	_root = _poolGameObjects.get();
	_root->SetName("Hierarchy");
	_root->AddMask(GO_MASK_ROOT_NODE);

	std::unique_ptr<ComponentTransform> transform = std::make_unique<ComponentTransform>();
	transform->SetOwner(_root.get());
	_root->AddComponent(std::move(transform));

	std::unique_ptr<GameObject> go = _poolGameObjects.get();
	go->SetName("Camera");
	transform = std::make_unique<ComponentTransform>();
	std::unique_ptr<ComponentCamera> cam = std::make_unique<ComponentCamera>(); cam->SetZNear(0.1f); cam->SetPosition(0, 0, 10); cam->SetZFar(250.0f);

	go->SetParent(_root.get());
	transform->SetOwner(go.get());
	cam->SetOwner(go.get());
	cam->ToggleMainCamera();
	cam->SetPosition(transform->GetPosition());
	

	go->AddComponent(std::move(transform));
	go->AddComponent(std::move(cam), GO_MASK_CAMERA);

	_mainCamera = static_cast<ComponentCamera *>(go->GetComponent(type_component::CAMERA));

	_root->AddChild(std::move(go));

	go = _poolGameObjects.get();
	go->SetName("Directional Light");
	transform = std::make_unique<ComponentTransform>();
	transform->SetOwner(go.get());
	std::unique_ptr<ComponentLight> light = std::make_unique<ComponentLight>(light_type::DIRECTIONAL_LIGHT);
	light->SetOwner(go.get());
	
	go->AddComponent(std::move(transform));
	go->AddComponent(std::move(light), GO_MASK_LIGHT);
	
	_root->AddChild(std::move(go));
	*/
	return true;
}

void ModuleScene::IterateRoot(GameObject& go) {

	for (auto const& children : go.GetChildren()) {
		IterateRoot(*children);
	}

	if (go.HasMesh()) {
		_objects.push_back(&go);
	}
	else if ((go.GetMask() & GO_MASK_CAMERA) != 0) _objectsToDraw.insert(&go);
	else if ((go.GetMask() & GO_MASK_LIGHT) != 0) _lights.push_back(&go);
}

bool ModuleScene::Start() {
	
	Load();

	return true;
}

void ModuleScene::FrustumCulling(OctreeNode* node) {

	if (_mainCamera->Intersects(node->_bounds)) {

		for (const auto& go : node->_gos) {
			OBB obb; go->GetOBB(obb);
			if (_mainCamera->Intersects(obb.MinimalEnclosingAABB())) {
				_objectsToDraw.insert(go);
			}
		}

		if (node->_children.size() != 0) { 
			for (int i = 0; i < 8; i++) {
				FrustumCulling(&(node->_children[i]));
			}
		}

	}
}

void ModuleScene::FrustumCulling() {
	for (const auto &go : _objects) {
		OBB obb; go->GetOBB(obb);
		if (_mainCamera->Intersects(obb.MinimalEnclosingAABB())) {
			_objectsToDraw.insert(go);
		}
	}
}

update_status ModuleScene::PreUpdate() {


	if( (_mask & LINEAR_AABB) != 0 ) { FrustumCulling(); }
	else if ((_mask & OCTREE) != 0) { 
		FrustumCulling(_octree->_root);
		if(_drawOctree) _octree->DebugDraw(_octree->_root);
	}

	return UPDATE_CONTINUE;
}

void ModuleScene::DrawRecursive(GameObject &go) {

	for (auto const& children : go.GetChildren()) {
		DrawRecursive(*children);
	}

	for (auto const& component : go.GetComponents()) {
		if (component->GetType() == type_component::MESHRENDERER) {
			static_cast<ComponentMeshRenderer*>(component.get())->Render();
		}
		component->DebugDraw();
	}

}

void ModuleScene::DrawFrustumOutput() {

	for (const auto& go : _objectsToDraw) {
		for (auto const& component : go->GetComponents()) {
			if (component->GetType() == type_component::MESHRENDERER) {
				static_cast<ComponentMeshRenderer*>(component.get())->Render();
			}
			component->DebugDraw();
		}
	}

	_objectsToDraw.clear();
	_objects.clear();
	_objects.shrink_to_fit();
	_lights.clear();
	_lights.shrink_to_fit();

	IterateRoot(*_root);

}

void ModuleScene::Draw() {
	if((_mask & NO_FRUSTUM) != 0) DrawRecursive(*_root);
	else DrawFrustumOutput();
}

void ModuleScene::SetMainCamera(ComponentCamera& mainCamera) {
	_mainCamera = &mainCamera;
}

void ModuleScene::SetDrawOctree(bool drawOctree) {
	_drawOctree = drawOctree;
}

void ModuleScene::UpdateMinMax(float3 min, float3 max) {

	if (min.x < _min.x) _min.x = min.x;
	if (min.y < _min.y) _min.y = min.y;
	if (min.z < _min.z) _min.z = min.z;

	if (max.x > _max.x) _max.x = max.x;
	if (max.y > _max.y) _max.y = max.y;
	if (max.z > _max.z) _max.z = max.z;

}

bool ModuleScene::Save() 
{
	_isSaving = true;
	Json::Value value;
	this->ToJson(value,0);
	Json::StyledWriter wr;
	std::ofstream ofs(DEFAULT_SCENE_PATH DEFAULT_SCENE_NAME DEFAULT_SCENE_EXT);
	std::string st = wr.write(value);
	ofs.write(st.c_str(), st.size());
	ofs.close();
	_isSaving = false;
	return true;
}

bool ModuleScene::Load() {
	_isLoading = true;
	std::ifstream ifs(DEFAULT_SCENE_PATH DEFAULT_SCENE_NAME DEFAULT_SCENE_EXT);
	Json::CharReaderBuilder reader;
	Json::Value obj;
	std::string error;

	if (!Json::parseFromStream(reader, ifs, &obj, &error))
	{
		LOG(_ERROR, "Error parsing file: %s", error);
		_isLoading = false;
		return false;
	}

	FromJson(obj[0]);

	_objects.clear();
	_lights.clear();

	IterateRoot(*_root);

	if (_octree) delete _octree;

	_octree = new Octree();
	_octree->_root->_bounds = AABB(float3(-100, -100, -100), float3(100, 100, 100));

	for (unsigned int i = 0; i < _objects.size(); ++i) {
		_octree->Insert(_octree->_root, _objects[i]);
	}

	_isLoading = false;

	return true;
}

bool ModuleScene::ToJson(Json::Value& value, int pos)
{
	Json::Value childrenList;
	_root->ToJson(childrenList, 0);
	value[pos][JSON_TAG_NAME] = DEFAULT_SCENE_NAME;
	value[pos][JSON_TAG_ROOT] = childrenList;
	App->editorCamera->GetCamera()->ToJson(value[pos][JSON_TAG_EDITOR_CAMERA], 0);

	return true;
}

bool ModuleScene::FromJson(const Json::Value& value) {

	Json::Value root = value[JSON_TAG_ROOT];

	if (!root.isNull()) {
		App->editorCamera->SetCamera(new ComponentCamera(value[JSON_TAG_EDITOR_CAMERA][0]));
		_root = _poolGameObjects.get();
		_root->FromJson(root[0]);
	}
	else {
		//TODO: JSON ERROR
	}
	return true;
}

void ModuleScene::GetLights(std::vector<GameObject*>& lights) {
	lights = _lights;
}

void ModuleScene::TogglePlay() {
	if (!_isPlaying) {
		if (!_isLoading) {
			if(Save()) _isPlaying = !_isPlaying;
		}
	}
	else {
		if (!_isSaving) {
			if (Load()) _isPlaying = !_isPlaying;
		}
	}
}
