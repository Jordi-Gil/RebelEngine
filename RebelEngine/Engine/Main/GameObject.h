#pragma once

#include "Components/Component.h"
#include <vector>

class GameObject
{

public:

	GameObject(){}

	void Update(){}

private:

	bool active = false;
	const char* name;

	std::unique_ptr<GameObject> parent;
	std::vector<std::unique_ptr<GameObject>> childrens;

	std::vector<std::unique_ptr<Component>> components;

};

