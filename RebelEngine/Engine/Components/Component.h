#pragma once

#include <memory>
#include "json/json.h"
#include "Utils/Globals.h"

class GameObject;

enum  class ComponentType {
	kNONE,
	kTRANSFORM,
	kMESHRENDERER,
	kCAMERA,
	kLIGHT
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

	virtual ComponentType GetType() const { return _type; }
	virtual GameObject* GetOwner() const { return _owner; }

	virtual void SetOwner(GameObject* go) { _owner = go; }

	virtual bool ToJson(Json::Value& value, int pos);
	virtual bool FromJson(const Json::Value& value);

protected:

	GameObject* _owner = nullptr;
	bool _active = false;
	ComponentType _type = ComponentType::kNONE;
	std::string _uuid;
};
