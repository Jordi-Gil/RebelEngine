#include "GameObject.h"
#include "Components/Component.h"

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
