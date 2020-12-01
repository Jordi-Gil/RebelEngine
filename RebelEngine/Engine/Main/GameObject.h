#pragma once

#include "Components/Component.h"
#include <vector>
#include <memory>
class GameObject
{

public:

	GameObject();
	//~GameObject() {}

	void Update(){}
	void AddChild(std::unique_ptr<GameObject>& go) { childrens.push_back(std::move(go)); }
	void AddComponent(std::unique_ptr<Component> comp) { components.push_back(std::move(comp)); }
private:

	bool active = false;
	const char* name;

	std::unique_ptr<GameObject> parent;
	std::vector<std::unique_ptr<GameObject>> childrens;

	std::vector<std::unique_ptr<Component>> components;

};

