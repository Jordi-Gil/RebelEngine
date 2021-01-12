#pragma once

#include "Components/Component.h"
#include "Components/ComponentMeshRenderer.h"

#include <vector>
#include <memory>

#include "Math/float4x4.h"

#include "json/json.h"

enum GAME_OBJECT_MASK {
	GO_MASK_NONE		= 1 << 1,
	GO_MASK_ROOT_NODE	= 1 << 2,
	GO_MASK_MESH		= 1 << 3,
	GO_MASK_CAMERA		= 1 << 4
};

class GameObject {

public:

	GameObject();
	GameObject(const char* name);
	GameObject(GameObject&& go);
	GameObject(const GameObject& go);
	GameObject(const Json::Value& value);

	void Update(){}
	void AddChild(std::unique_ptr<GameObject>&& go);
	void AddComponent(std::unique_ptr<Component>&& comp, GAME_OBJECT_MASK mask = GO_MASK_NONE);
	void SetName(const char* name);
	void SetParent(GameObject* go);
	void AddMask(GAME_OBJECT_MASK mask);
	bool HasComponent(type_component type) const;
	bool HasMesh() const {  return (_mask & GO_MASK_MESH) != 0; }

	void UpdateChildrenTransform();
	bool ToJson(Json::Value& value, int pos);
	bool FromJson(const Json::Value& value);

	void ToggleSelected();

#pragma region getters
	const char* GetName() const { return _name.c_str(); }
	int GetNumChildren() const { return _children.size(); };
	GameObject* GetParent() const { return _parent; }
	Component* GetComponent(type_component type) const;
	std::vector<std::unique_ptr<Component>>& GetComponents() { return _components; }
	const std::vector<std::unique_ptr<GameObject>>& GetChildren() const { return _children; }
	std::vector<std::unique_ptr<GameObject>>& GetChildren() { return _children; }
	float4x4 GetGlobalMatrix() const;
	float4x4 GetLocalMatrix() const;
	uint32_t GetMorton() const;
	void GetAABB(AABB& aabb) const;
	void GetOBB(OBB& obb) const;
	bool IsSelected() const { return _selected; };
	int GetMask() const { return _mask; }
	std::string GetUUID() { return _uuid; }

#pragma endregion getters

#pragma region operators

	GameObject& operator=(const GameObject& go);

#pragma endregion operators

private:

	std::string _name = "";

	bool _active = false;
	bool _selected = false;

	int _mask = GO_MASK_NONE;

	GameObject *_parent = nullptr;
	ComponentMeshRenderer* _meshRenderer = nullptr;

	std::vector<std::unique_ptr<GameObject>> _children;
	std::vector<std::unique_ptr<Component>> _components;

	std::string _uuid;

public: 

	friend class GUIHierarchy;

};
