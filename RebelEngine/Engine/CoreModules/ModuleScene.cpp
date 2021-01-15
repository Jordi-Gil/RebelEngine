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
	this->FromJson(value);
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

	/*_root = std::make_unique<GameObject>("Hierarchy");
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

	cam->SetPosition(transform->GetPosition());

	go->AddComponent(std::move(transform));
	go->AddComponent(std::move(cam), GO_MASK_CAMERA);

	_mainCamera = static_cast<ComponentCamera *>(go->GetComponent(type_component::CAMERA));

	_root->AddChild(std::move(go));*/

	go = _poolGameObjects.get();
	go->SetName("Directional Light");
	transform = std::make_unique<ComponentTransform>();
	transform->SetOwner(go.get());
	std::unique_ptr<ComponentLight> light = std::make_unique<ComponentLight>(light_type::DIRECTIONAL_LIGHT);
	light->SetOwner(go.get());

	go->AddComponent(std::move(transform));
	go->AddComponent(std::move(light));

	_root->AddChild(std::move(go));

	return true;
}

void ModuleScene::IterateRoot(GameObject& go) {

	for (auto const& children : go.GetChildren()) {
		IterateRoot(*children);
	}

	if (go.HasMesh()) { 
		_objects.push_back(&go);
		OBB obb; go.GetOBB(obb);
		UpdateMinMax(obb.MinimalEnclosingAABB().minPoint, obb.MinimalEnclosingAABB().maxPoint);
	}
	else if (go.GetMask() & GO_MASK_CAMERA) _objectsToDraw.push_back(&go);
}

bool ModuleScene::Start() {
	
	//Load();

	IterateRoot(*_root);

	_octree = new Octree();
	_octree->_root->_bounds = AABB(_min, _max);

	for (int i = 0; i < _objects.size(); ++i) {
		_octree->Insert(_octree->_root, _objects[i]);
	}

	return true;
}

void ModuleScene::FrustumCulling(OctreeNode* node) {

	if (_mainCamera->Intersects(node->_bounds)) {

		for (const auto& go : node->_gos) {
			AABB box; go->GetAABB(box);
			OBB obb = box.Transform(go->GetGlobalMatrix());
			if (_mainCamera->Intersects(obb.MinimalEnclosingAABB())) {
				_objectsToDraw.push_back(go);
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
		AABB box;  go->GetAABB(box);  OBB obb = box.Transform(go->GetGlobalMatrix());
		if (_mainCamera->Intersects(obb.MinimalEnclosingAABB())) {
			_objectsToDraw.push_back(go);
		}
	}
}

update_status ModuleScene::PreUpdate() {

	
	if( (_mask & LINEAR_AABB) != 0 ) { FrustumCulling(); }
	else if ((_mask & OCTREE) != 0) { 
		FrustumCulling(_octree->_root);
		_octree->DebugDraw(_octree->_root);
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
	_objectsToDraw.shrink_to_fit();
	_objects.clear();
	_objects.shrink_to_fit();

	IterateRoot(*_root);

}

void ModuleScene::Draw() {
	if((_mask & NO_FRUSTUM) != 0) DrawRecursive(*_root);
	else DrawFrustumOutput();
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
	Json::Value value;
	this->ToJson(value,0);
	Json::StyledWriter wr;
	std::ofstream ofs(DEFAULT_SCENE_PATH DEFAULT_SCENE_NAME DEFAULT_SCENE_EXT);
	std::string st = wr.write(value);
	ofs.write(st.c_str(), st.size());

	return true;
}

bool ModuleScene::Load() {

	std::ifstream ifs(DEFAULT_SCENE_PATH DEFAULT_SCENE_NAME DEFAULT_SCENE_EXT);
	Json::CharReaderBuilder reader;
	Json::Value obj;
	std::string error;

	if (!Json::parseFromStream(reader, ifs, &obj, &error))
	{
		LOG(_ERROR, "Error parsing file: %s", error);
		return false;
	}

	FromJson(obj[0]);

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
		_root = std::make_unique<GameObject>(root[0]);
	}
	else {
		//TODO: JSON ERROR
	}
	return true;
}
