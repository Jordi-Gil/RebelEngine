#pragma once

#include "Components/Component.h"
#include <vector>
#include <memory>

#include "Math/float4x4.h"

class GameObject 
{

public:

	GameObject() {}
	GameObject(const char* _name);
	GameObject(GameObject&& _go);

	void Update(){}
	void AddChild(std::unique_ptr<GameObject>&& go);
	void AddComponent(std::unique_ptr<Component>&& comp);
	void SetName(const char* _name);
	void SetParent(GameObject* _go);
	bool HasComponent(type_component _type) const;
	void EraseChildrenNull();

	const char* GetName() const { return name; }
	int GetNumChildren() const { return children.size(); };
	GameObject* GetParent() const { return parent; }
	Component* GetComponent(type_component _type) const;
	
	const std::vector<std::unique_ptr<GameObject>>& GetChildren() const { return children; }
	std::vector<std::unique_ptr<GameObject>>& GetChildren() { return children; }
	const float4x4 GetGlobalMatrix() const;

private:

	bool active = false;
	const char* name = "";
	
	GameObject *parent = nullptr;
	std::vector<std::unique_ptr<GameObject>> children;
	std::vector<std::unique_ptr<Component>> components;

public: 

	friend class GUIHierarchy;

};
