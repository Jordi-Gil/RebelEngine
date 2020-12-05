#include "GameObject.h"
#include "Components/Component.h"

GameObject::GameObject(){
	
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

bool GameObject::HasComponent(type_component _type) const {

	for (auto const &component : components) {
		if (component->GetType() == type_component::MESHRENDERER) return true;
	}

	return false;

}

Component* GameObject::GetComponent(type_component type) const
{
	for (auto const& component : components) {
		if (component->GetType() == type) return component.get();
	}
}
