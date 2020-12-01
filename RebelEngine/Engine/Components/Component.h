#pragma once

#include <memory>

class GameObject;

enum class type_component {
	TRANSFORM,
	MESHRENDERER
};

class Component {

public:

	Component() {}

	virtual ~Component() {}

	virtual void Enable() {}
	virtual void Update() {}
	virtual void Disable() {}

protected:

	GameObject* owner;
	bool active = false;
	type_component type;

};