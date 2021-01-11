#pragma once

#include "Components/Component.h"
#include "Components/ComponentMeshRenderer.h"

#include <vector>
#include <memory>

#include "Math/float4x4.h"

#include "json/json.h"

class GameObject {

public:

	GameObject();
	GameObject(const char* name);
	GameObject(GameObject&& go);
	GameObject(const Json::Value& value);
	~GameObject();

	void Update(){}
	void AddChild(std::unique_ptr<GameObject>&& go);
	void AddComponent(std::unique_ptr<Component>&& comp);
	void SetName(const char* name);
	void SetParent(GameObject* go);
	bool HasComponent(type_component type) const;
	void EraseChildrenNull();
	void UpdateChildrenTransform();
	bool ToJson(Json::Value& value, int pos);
	bool FromJson(const Json::Value& value);

#pragma region getters
	const char* GetName() const { return _name; }
	int GetNumChildren() const { return _children.size(); };
	GameObject* GetParent() const { return _parent; }
	Component* GetComponent(type_component type) const;
	std::vector<std::unique_ptr<Component>>& GetComponents() { return _components; }
	const std::vector<std::unique_ptr<GameObject>>& GetChildren() const { return _children; }
	std::vector<std::unique_ptr<GameObject>>& GetChildren() { return _children; }
	const float4x4 GetGlobalMatrix() const;
	uint32_t GetMorton() const;
	bool HasMesh() const { return _hasMesh; }
	std::string GetUUID() { return _uuid; }

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
	std::string _uuid;

public: 

	friend class GUIHierarchy;

};
