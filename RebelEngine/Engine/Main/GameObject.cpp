#include "GameObject.h"

#include "CoreModules/ModuleScene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"
#include "Components/Component.h"

#include "Application.h"

#include "Utils/RUUID.h"

GameObject::GameObject() {
	_uuid = RUUID::generate_uuid_v4();
}

GameObject::GameObject(const char* name) {
	_name = std::string(name);
	_uuid = RUUID::generate_uuid_v4();
}

GameObject::GameObject(const GameObject& go) {

	_active = go._active;
	_name = go._name;
	_parent = go._parent;
	_uuid = RUUID::generate_uuid_v4();

	for (const auto& child : go._children) {
		std::unique_ptr<GameObject> aux = App->scene->_poolGameObjects.get();
		*aux = *child;
		_children.push_back(std::move(aux));
	}

	for (const auto& component : go._components) {
		_components.push_back(std::make_unique<Component>(*component));
	}

	_mask = go._mask;

}

GameObject::GameObject(GameObject&& go) {

	_active = go._active;
	_name = go._name;
	_parent = go._parent;
	_uuid = RUUID::generate_uuid_v4();

	go._parent = nullptr;
	go._name = nullptr;

	for (auto it = go._children.begin(); it != go._children.end(); ++it) {
		(*it)->_parent = this;
		_children.emplace_back(std::move(*it));
	}

	go._children.clear();
	go._children.shrink_to_fit();

	for (auto it = go._components.begin(); it != go._components.end(); ++it) {
		(*it)->SetOwner(this);
		_components.emplace_back(std::move(*it));
	}
	go._components.clear();
	go._components.shrink_to_fit();
	
}

GameObject::GameObject(const Json::Value& value) {
	FromJson(value);
}

void GameObject::AddChild(std::unique_ptr<GameObject>&& go){
	_children.push_back(std::move(go));
}

void GameObject::AddComponent(std::unique_ptr<Component>&& comp, GAME_OBJECT_MASK mask) {
	_mask |= mask;
	_components.push_back(std::move(comp)); 
	if (_mask & GO_MASK_MESH) _meshRenderer = (ComponentMeshRenderer *) static_cast<ComponentMeshRenderer*>(_components.rbegin()->get());
}

void GameObject::SetName(const char* name) {
	_name = std::string(name);
}

void GameObject::SetParent(GameObject* go) {
	_parent = go;
}

void GameObject::AddMask(GAME_OBJECT_MASK mask){ 
	_mask |= mask;
}

bool GameObject::HasComponent(ComponentType type) const {

	for (auto const &component : _components) {
		if (component->GetType() == type) return true;
	}

	return false;
}

Component* GameObject::GetComponent(ComponentType type) const {
	for (auto const& component : _components) {
		if (component->GetType() == type) return component.get();
	}
	return nullptr;
}

float4x4 GameObject::GetGlobalMatrix() const {
	ComponentTransform* transform = static_cast<ComponentTransform*>(GetComponent(ComponentType::kTRANSFORM));
	return transform->GetGlobalMatrix();
}

float4x4 GameObject::GetLocalMatrix() const {
	ComponentTransform* transform = static_cast<ComponentTransform*>(GetComponent(ComponentType::kTRANSFORM));
	return transform->GetLocalMatrix();
}

uint32_t GameObject::GetMorton() const {
	return _meshRenderer->GetMorton();
}

void GameObject::GetAABB(AABB& aabb) const {
	if (_meshRenderer) _meshRenderer->GetAABB(aabb);
	else aabb.SetNegativeInfinity();
}

void GameObject::GetOBB(OBB& obb) const {
	AABB box; GetAABB(box);
	obb = box;
	obb.Transform(GetGlobalMatrix());
}

GameObject& GameObject::operator=(const GameObject& go) {

	_active = go._active;
	_name = go._name;
	_parent = go._parent;
	_uuid = RUUID::generate_uuid_v4();

	_children.clear();
	for (const auto& child : go._children) {
		std::unique_ptr<GameObject> aux = App->scene->_poolGameObjects.get();
		*aux = *child;
		this->_children.push_back(std::move(aux));
	}

	_components.clear();
	for (const auto& component : go._components) {
		switch (component->GetType())
		{
		case ComponentType::kCAMERA:{
			ComponentCamera* compAux = (ComponentCamera*)(component.get());
			_components.push_back(std::make_unique<ComponentCamera>(*compAux));
			break;
			}
		case ComponentType::kMESHRENDERER: {
			ComponentMeshRenderer* compAux = (ComponentMeshRenderer*)(component.get());
			_components.push_back(std::make_unique<ComponentMeshRenderer>(*compAux));
			break;
		}
		case ComponentType::kTRANSFORM: {
			ComponentTransform* compAux = (ComponentTransform*)(component.get());
			_components.push_back(std::make_unique<ComponentTransform>(*compAux));
			break;
		}
		case ComponentType::kLIGHT: {
			ComponentLight* compAux = (ComponentLight*)(component.get());
			_components.push_back(std::make_unique<ComponentLight>(*compAux));
			break;
		}
		default:
			_components.push_back(std::make_unique<Component>(*component));
			break;
		}
		
	}

	_mask = go._mask;

	return *this;
}

void UpdateChildrenTransform_rec(GameObject& go) {

	ComponentTransform* comp = (ComponentTransform*)go.GetComponent(ComponentType::kTRANSFORM);
	comp->UpdateGlobalMatrix();
	for (auto const& child : go.GetChildren()) {
		UpdateChildrenTransform_rec(*child);
	}
}

void GameObject::DeleteChild(std::unique_ptr<GameObject>& go) {

	auto it = std::find(_children.begin(), _children.end(), go);

	if (it != _children.end()) 
		_children.erase(it);

}

void GameObject::DeleteMarkedChildren() {

	for (int i = 0; i < _children.size(); i++) {
		if (_children[i]->_delete) {
			auto it = std::find(_children.begin(), _children.end(), _children[i]);
			_children.erase(it);
		}
		else {
			_children[i]->DeleteMarkedChildren();
		}
	}

}

//Call it when loading GameObject from FBX
void GameObject::CollapseChildIntoParent() {
	if (_children.size() == 1) {

		if (_children[0]->_children.size() != 0) _children[0]->CollapseChildIntoParent();

		_components.clear();
		for (int j = 0; j < _children[0]->_components.size(); j++) {
			_components.push_back(std::move(_children[0]->_components[j]));
			_components.rbegin()->get()->SetOwner(this);
		}
		_meshRenderer = _children[0]->_meshRenderer;
		_mask = _children[0]->_mask;
		_children.clear();
	}
	else {
		for (int i = 0; i < _children.size(); i++) {
			_children[i]->CollapseChildIntoParent();
		}
	}
}

void GameObject::UpdateChildrenTransform() {
	UpdateChildrenTransform_rec(*this);
}

void GameObject::ToggleSelected(){
	_selected = !_selected;
}

bool GameObject::ToJson(Json::Value& value, int pos)  {

	Json::Value childrenList;
	Json::Value componentList;

	value[pos][JSON_TAG_NAME] = _name;
	value[pos][JSON_TAG_UUID] = _uuid;

	int go_cont = 0;
	for (const auto& child : _children)
	{
		child->ToJson(childrenList, go_cont);
		++go_cont;
	}

	int comp_cont = 0;
	for (const auto& comp : _components)
	{
		comp->ToJson(componentList, comp_cont);
		++comp_cont;
	}

	value[pos][JSON_TAG_GAMEOBJECTS] = childrenList;
	value[pos][JSON_TAG_COMPONENTS] = componentList;

	return true;
}

bool GameObject::FromJson(const Json::Value& value)  {
	
	if (!value.isNull()) 
	{
		_name = _strdup(value[JSON_TAG_NAME].asCString());
		_uuid = value[JSON_TAG_UUID].asCString();
		_active = value[JSON_TAG_ACTIVE].asBool();

		for (Json::Value jsonComp : value[JSON_TAG_COMPONENTS])
		{
			ComponentType type = (ComponentType) jsonComp[JSON_TAG_TYPE].asInt();
			std::unique_ptr<Component> comp;
			switch (type)
			{
				case ComponentType::kNONE:
				{
					comp = std::make_unique<Component>(jsonComp);
					AddMask(GAME_OBJECT_MASK::GO_MASK_NONE);
					break;
				}
				case ComponentType::kCAMERA:
				{
					comp = std::make_unique<ComponentCamera>(jsonComp);
					AddMask(GAME_OBJECT_MASK::GO_MASK_CAMERA);
					ComponentCamera* aux = (ComponentCamera*)comp.get();
					if (aux->IsMainCamera())
						App->scene->SetMainCamera(*aux);
					break;
				}
				case ComponentType::kMESHRENDERER:
				{
					comp = std::make_unique<ComponentMeshRenderer>(jsonComp);
					AddMask(GAME_OBJECT_MASK::GO_MASK_MESH);
					_meshRenderer = static_cast<ComponentMeshRenderer*>(comp.get());
					break;
				}
				case ComponentType::kTRANSFORM: {
					comp = std::make_unique<ComponentTransform>(jsonComp);
					break;
				}
				case ComponentType::kLIGHT:{
					comp = std::make_unique<ComponentLight>(jsonComp);
					AddMask(GAME_OBJECT_MASK::GO_MASK_LIGHT);
					break;
				}
			}

			comp->SetOwner(this);

			AddComponent(std::move(comp));
		}

		for (Json::Value jsonGo : value[JSON_TAG_GAMEOBJECTS]) {

			std::unique_ptr<GameObject> go = App->scene->_poolGameObjects.get();
			go->SetParent(this);
			go->FromJson(jsonGo);
			AddChild(std::move(go));
		}
		
	}
	else {
		LOG(_ERROR, "Error loading Jvalue");
	}
	return true;
}
