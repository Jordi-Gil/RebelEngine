#pragma once

#include "Components/Component.h"
#include <vector>
#include <memory>

class GameObject 
{

public:

	GameObject();

	void Update(){}
	void AddChild(std::unique_ptr<GameObject>& go) { children.push_back(std::move(go)); }
	void AddComponent(std::unique_ptr<Component> comp) { components.push_back(std::move(comp)); }
	void SetName(const char* _name) { name = _name; }
	const char* GetName() { return name; }
	const int GetNumChildren() { return children.size(); };
	//const std::vector<std::unique_ptr<GameObject>>& GetChildren() const { return children; }
	std::vector<std::unique_ptr<GameObject>>::iterator begin() {
		return children.begin();
	}
	std::vector<std::unique_ptr<GameObject>>::iterator end() {
		return children.end();
	}
private:

	bool active = false;
	const char* name;

	std::unique_ptr<GameObject> parent;
	std::vector<std::unique_ptr<GameObject>> children;
	std::vector<std::unique_ptr<Component>> components;

public: 
	friend class GUIHierarchy;
};
