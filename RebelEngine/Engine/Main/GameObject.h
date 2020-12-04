#pragma once

#include "Components/Component.h"
#include <vector>
#include <memory>

class GameObject 
{

public:

	GameObject();

	void Update(){}
	void AddChild(std::unique_ptr<GameObject>&& go);
	void AddComponent(std::unique_ptr<Component>&& comp);
	void SetName(const char* _name);

	bool HasComponent(type_component _type) const;

	const char* GetName() const { return name; }
	int GetNumChildren() const { return children.size(); };
	
	const std::vector<std::unique_ptr<GameObject>>& GetChildren() const { return children; }
	Component& GetComponent(type_component type) const;

private:

	bool active = false;
	const char* name;

	std::unique_ptr<GameObject> parent;
	std::vector<std::unique_ptr<GameObject>> children;
	std::vector<std::unique_ptr<Component>> components;

public: 
	friend class GUIHierarchy;
};
