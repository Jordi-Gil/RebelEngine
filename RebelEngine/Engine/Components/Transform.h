#pragma once
#include "Component.h"
class Transform : public Component {

public:

	Transform() {}
	~Transform() {}

	void Enable();

	void Update();

	void Disable();



};

