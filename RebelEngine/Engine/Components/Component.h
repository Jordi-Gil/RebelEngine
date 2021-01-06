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


	virtual ~Component() = default;

	virtual void Enable() {}
	virtual void Update() {}
	virtual void Disable() {}
	virtual type_component GetType() const { return _type; }

	virtual void SetOwner(GameObject* go) { _owner = go; }

	virtual bool ToJson(Json::Value& value, int pos);

protected:

	GameObject* _owner = nullptr;
	bool _active = false;
	type_component _type = type_component::NONE;
	std::string _uuid;
};
