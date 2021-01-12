#include "GameObject.h"

#include "Components/Component.h"
#include "Components/ComponentTransform.h"

#include "Application.h"

#include "CoreModules/ModuleScene.h"

GameObject::GameObject(const char* name) {
	_name = std::string(name);
}

GameObject::GameObject(const GameObject& go) {

	this->_active = go._active;
	this->_name = go._name;
	this->_parent = go._parent;

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

	this->_active = go._active;
	this->_name = go._name;
	this->_parent = go._parent;

	go._parent = nullptr;
	go._name = nullptr;

	for (auto it = go._children.begin(); it != go._children.end(); ++it) {
		(*it)->_parent = this;
		this->_children.emplace_back(std::move(*it));
	}
	go._children.clear();
	go._children.shrink_to_fit();

	for (auto it = go._components.begin(); it != go._components.end(); ++it) {
		(*it)->SetOwner(this);
		this->_components.emplace_back(std::move(*it));
	}
	go._components.clear();
	go._components.shrink_to_fit();
	
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

Component* GameObject::GetComponent(type_component type) const
{
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
		this->_components.push_back(std::make_unique<Component>(*component));
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
