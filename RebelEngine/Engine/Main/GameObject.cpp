#include "GameObject.h"

#include "Components/Component.h"
#include "Components/ComponentTransform.h"

GameObject::GameObject(const char* _name) {
	name = _name;
}

void GameObject::EraseChildrenByName(const char* name) {
	bool found = false;
	auto it_found = children.begin();
	for (auto it = children.begin();!found && it != children.end(); ++it) {
		if (it->get()->name == name) {
			found = true;
			it_found = it;
		}
	}
	if (found) children.erase(it_found);
}

void GameObject::AddChild(std::unique_ptr<GameObject>&& go){
	children.push_back(std::move(go));
}

void GameObject::AddComponent(std::unique_ptr<Component>&& comp) { 
	components.push_back(std::move(comp)); 
}

void GameObject::SetName(const char* _name) {
	name = _name;
}

void GameObject::SetParent(GameObject* _go) {
	parent = _go;
}

bool GameObject::HasComponent(type_component _type) const {

	for (auto const &component : components) {
		if (component->GetType() == _type) return true;
	}

	return false;

}

Component* GameObject::GetComponent(type_component type) const
{
	for (auto const& component : components) {
		if (component->GetType() == type) return component.get();
	}
	return nullptr;
}

const float4x4 GameObject::GetGlobalMatrix() const
{
	ComponentTransform* transform = static_cast<ComponentTransform*>(GetComponent(type_component::TRANSFORM));
	return transform->GetGlobalMatrix();
}
