#pragma once

#include "Component.h"

#include "Math/float4x4.h"
#include "Math/Quat.h"

class Transform : public Component {

public:

	Transform() {}
	Transform(const float3 _position, const float3 _rotation, const float3 _scale);
	~Transform() {}

	void SetTransform(const float3 _position, const float3 _rotation, const float3 _scale);

	void Enable() {}

	void Update() {}

	void Disable() {}

private:

	float3 position = float3::zero;
	float3 rotation = float3::zero;
	float3 scale = float3::one;

	float4x4 localMatrix = float4x4::identity;

	Quat rotationQuat = Quat::identity;

};

