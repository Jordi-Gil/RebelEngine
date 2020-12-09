#pragma once

#include <memory>

class GameObject;

enum  class type_component {
	NONE,
	TRANSFORM,
	MESHRENDERER,
	CAMERA
};

class Component {

public:

	Component() {}

	virtual ~Component() = default;

	virtual void Enable() {}
	virtual void Update() {}
	virtual void Disable() {}
	virtual type_component GetType() const { return type; }

	virtual void SetOwner(GameObject* go) { owner = go; }

protected:

	GameObject* owner = nullptr;
	bool active = false;
	type_component type = type_component::NONE;

};
