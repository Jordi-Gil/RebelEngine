#include "GameObject.h"

#include "CoreModules/ModuleScene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentCamera.h"
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

	this->_active = go._active;
	this->_name = go._name;
	this->_parent = go._parent;
	this->_uuid = go._uuid;

	for (const auto& child : go._children) {
		std::unique_ptr<GameObject> aux = App->scene->_poolGameObjects.get();
		*aux = *child;
		this->_children.push_back(std::move(aux));
	}

	for (const auto& component : go._components) {
		this->_components.push_back(std::make_unique<Component>(*component));
	}

	this->_mask = go._mask;

}

GameObject::GameObject(GameObject&& go) {

	_active = go._active;
	_name = go._name;
	_parent = go._parent;
	_uuid = go._uuid;

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

bool GameObject::HasComponent(type_component type) const {

	for (auto const &component : _components) {
		if (component->GetType() == type) return true;
	}

	return false;
}

Component* GameObject::GetComponent(type_component type) const {
	for (auto const& component : _components) {
		if (component->GetType() == type) return component.get();
	}
	return nullptr;
}

float4x4 GameObject::GetGlobalMatrix() const {
	ComponentTransform* transform = static_cast<ComponentTransform*>(GetComponent(type_component::TRANSFORM));
	return transform->GetGlobalMatrix();
}

float4x4 GameObject::GetLocalMatrix() const {
	ComponentTransform* transform = static_cast<ComponentTransform*>(GetComponent(type_component::TRANSFORM));
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
	obb = box.Transform(GetGlobalMatrix());
}

GameObject& GameObject::operator=(const GameObject& go) {

	this->_active = go._active;
	this->_name = go._name;
	this->_parent = go._parent;

	for (const auto& child : go._children) {
		std::unique_ptr<GameObject> aux = App->scene->_poolGameObjects.get();
		*aux = *child;
		this->_children.push_back(std::move(aux));
	}

	for (const auto& component : go._components) {
		switch (component->GetType())
		{
		case type_component::CAMERA:{
			ComponentCamera* compAux = (ComponentCamera*)(component.get());
			this->_components.push_back(std::make_unique<ComponentCamera>(*compAux));
			break;
			}
		case type_component::MESHRENDERER: {
			ComponentMeshRenderer* compAux = (ComponentMeshRenderer*)(component.get());
			this->_components.push_back(std::make_unique<ComponentMeshRenderer>(*compAux));
			break;
		}
		case type_component::TRANSFORM: {
			ComponentTransform* compAux = (ComponentTransform*)(component.get());
			this->_components.push_back(std::make_unique<ComponentTransform>(*compAux));
			break;
		}
		default:
			this->_components.push_back(std::make_unique<Component>(*component));
			break;
		}
		
	}

	this->_mask = go._mask;

	return *this;
}

void UpdateChildrenTransform_rec(GameObject& go) {

	ComponentTransform* comp = (ComponentTransform*)go.GetComponent(type_component::TRANSFORM);
	comp->UpdateGlobalMatrix();
	for (auto const& child : go.GetChildren()) {
		UpdateChildrenTransform_rec(*child);
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
			type_component type = (type_component)jsonComp[JSON_TAG_TYPE].asInt();
			std::unique_ptr<Component> comp;
			switch (type)
			{
			case type_component::NONE: comp = std::make_unique<Component>(jsonComp); AddMask(GAME_OBJECT_MASK::GO_MASK_NONE);   break;
			case type_component::CAMERA:
			{
				comp = std::make_unique<ComponentCamera>(jsonComp);
				AddMask(GAME_OBJECT_MASK::GO_MASK_CAMERA);
				ComponentCamera* aux = (ComponentCamera*)comp.get();
				if (aux->IsMainCamera())
					App->scene->SetMainCamera(*aux);
				break;
			}
			case type_component::MESHRENDERER:
			{
				comp = std::make_unique<ComponentMeshRenderer>(jsonComp);
				AddMask(GAME_OBJECT_MASK::GO_MASK_MESH);
				_meshRenderer = static_cast<ComponentMeshRenderer*>(comp.get());
				break;
			}
			case type_component::TRANSFORM: comp = std::make_unique<ComponentTransform>(jsonComp); break;
			}
			comp->SetOwner(this);

			AddComponent(std::move(comp));
		}

		for (Json::Value jsonGo : value[JSON_TAG_GAMEOBJECTS])
		{
			std::unique_ptr<GameObject> go = App->scene->_poolGameObjects.get();
			go->SetParent(this);
			go->FromJson(jsonGo);
			AddChild(std::move(go));
		}

		
	}
	else {
		//TODO: JSON ERROR
	}
	return true;
}
