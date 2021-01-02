#include "GameObject.h"

#include "Components/Component.h"
#include "Components/ComponentTransform.h"

#include "Application.h"
#include "CoreModules/ModuleResourceManagement.h"

GameObject::GameObject(const char* name) {
	_name = _strdup(name);
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

GameObject::~GameObject() {
	free((char*)_name);
	_name = nullptr;
}

//TODO: Modify to UUID
void GameObject::EraseChildrenNull() {
	bool found = false;
	auto it_found = _children.begin();
	for (auto it = _children.begin(); !found && it != _children.end(); ++it) {
		if (!it->get()->_name) { //null if is destroyed
			found = true;
			it_found = it;
		}
	}
	if (found) 
		_children.erase(it_found);
}

void GameObject::AddChild(std::unique_ptr<GameObject>&& go){
	_children.push_back(std::move(go));
}

void GameObject::AddComponent(std::unique_ptr<Component>&& comp) { 
	_components.push_back(std::move(comp)); 
}

void GameObject::SetName(const char* name) {
	_name = _strdup(name);
}

void GameObject::SetParent(GameObject* go) {
	_parent = go;
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

const float4x4 GameObject::GetGlobalMatrix() const
{
	ComponentTransform* transform = static_cast<ComponentTransform*>(GetComponent(type_component::TRANSFORM));
	return transform->GetGlobalMatrix();
}

uint32_t GameObject::GetMorton() const {
	auto comp = (ComponentMeshRenderer *) GetComponent(type_component::MESHRENDERER);
	return comp->GetMorton();
}

GameObject& GameObject::operator=(const GameObject& go) {

	this->_active = go._active;
	this->_name = go._name;
	this->_parent = go._parent;

	for (const auto& child : go._children) {
		std::unique_ptr<GameObject> aux = App->resourcemanager->_poolGameObjects.get();
		*aux = *child;
		this->_children.push_back(std::move(aux));
	}

	for (const auto& component : go._components) {
		this->_components.push_back(std::make_unique<Component>(*component));
	}

	this->_hasMesh = go._hasMesh;

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
