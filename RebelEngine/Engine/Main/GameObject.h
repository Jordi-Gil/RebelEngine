#pragma once

#include "Components/Component.h"
#include "Components/ComponentMeshRenderer.h"

#include <vector>
#include <memory>

#include "Math/float4x4.h"

class GameObject {

public:

	GameObject() {}
	GameObject(const char* name);
	GameObject(GameObject&& go);

	~GameObject();

	void Update(){}
	void AddChild(std::unique_ptr<GameObject>&& go);
	void AddComponent(std::unique_ptr<Component>&& comp);
	void SetName(const char* name);
	void SetParent(GameObject* go);
	bool HasComponent(type_component type) const;
	void EraseChildrenNull();
	void UpdateChildrenTransform();

#pragma region getters
	const char* GetName() const { return _name; }
	int GetNumChildren() const { return _children.size(); };
	GameObject* GetParent() const { return _parent; }
	Component* GetComponent(type_component type) const;
	const std::vector<std::unique_ptr<GameObject>>& GetChildren() const { return _children; }
	std::vector<std::unique_ptr<GameObject>>& GetChildren() { return _children; }
	const float4x4 GetGlobalMatrix() const;
	uint32_t GetMorton() const;
	bool HasMesh() const { return _hasMesh; }
#pragma endregion getters

#pragma region operators

	GameObject& operator=(const GameObject& go);

#pragma endregion operators

private:

	bool _active = false;
	char* _name = nullptr;
	
	GameObject *_parent = nullptr;
	std::vector<std::unique_ptr<GameObject>> _children;

	std::vector<std::unique_ptr<Component>> _components;

	bool _hasMesh = false;

public: 

	friend class GUIHierarchy;

};
