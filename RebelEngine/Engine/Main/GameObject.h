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
	GameObject(GameObject&& _go) {

		this->active = _go.active;
		this->name = _go.name;
		this->parent = _go.parent;

		_go.parent = nullptr;
		_go.name = nullptr;

		for (auto it = _go.children.begin(); it != _go.children.end(); ++it) {
			this->children.emplace_back(std::move(*it));
		}
		_go.children.clear();
		_go.children.shrink_to_fit();

		for (auto it = _go.components.begin(); it != _go.components.end(); ++it) {
			this->components.emplace_back(std::move(*it));
		}
		_go.components.clear();
		_go.components.shrink_to_fit();
	}

	void Update(){}
	void AddChild(std::unique_ptr<GameObject>&& go);
	void AddComponent(std::unique_ptr<Component>&& comp);
	void SetName(const char* _name);
	void SetParent(GameObject* _go);
	bool HasComponent(type_component _type) const;
	void EraseChildrenByName(const char* name);

	const char* GetName() const { return name; }
	int GetNumChildren() const { return children.size(); };
	GameObject* GetParent() const { return parent; }
	Component* GetComponent(type_component _type) const;
	
	const std::vector<std::unique_ptr<GameObject>>& GetChildren() const { return children; }
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
