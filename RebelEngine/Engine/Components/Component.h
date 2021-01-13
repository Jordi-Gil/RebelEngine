#pragma once

#include <memory>
#include "json/json.h"
#include "Utils/Globals.h"

class GameObject;

enum  class type_component {
	NONE,
	TRANSFORM,
	MESHRENDERER,
	CAMERA
};

class Component {

public:

	Component();
	Component(const Component& comp);
	Component(const Json::Value& value);

	virtual ~Component() = default;

	virtual void Enable() {}
	virtual void Update() {}
	virtual void Disable() {}
	virtual void DebugDraw() {}
	virtual void OnEditor() {};
	virtual type_component GetType() const { return _type; }

	virtual void SetOwner(GameObject* go) { _owner = go; }

	virtual bool ToJson(Json::Value& value, int pos);
	virtual bool FromJson(const Json::Value& value);

protected:

	GameObject* _owner = nullptr;
	bool _active = false;
	type_component _type = type_component::NONE;
	std::string _uuid;
};
