#pragma once

#include <memory>

class GameObject;

enum  class type_component {
	TRANSFORM,
	MESHRENDERER
};

class Component {

public:

	Component() {}

	virtual ~Component() = default;

	virtual void Enable() {}
	virtual void Update() {}
	virtual void Disable() {}
	virtual type_component GetType() const { return type; }

protected:

	GameObject* owner;
	bool active = false;
	type_component type;
};
