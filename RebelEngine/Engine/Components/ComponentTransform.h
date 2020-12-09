#pragma once

#include "Component.h"

#include "Math/float4x4.h"
#include "Math/Quat.h"
#include "assimp/matrix4x4.h"
class ComponentTransform : public Component {

public:

	ComponentTransform() {}
	ComponentTransform(const float3 _position, const float3 _rotation, const float3 _scale);
	~ComponentTransform() {}

	void SetTransform(const float3 _position, const float3 _rotation, const float3 _scale);
	void SetTransform(const aiMatrix4x4& matrix);
	void UpdateTransform(const ComponentTransform& old_father, const ComponentTransform& new_father);

	void Enable() override {}

	void Update() override {}

	void Disable() override {}

	float3 GetPosition() const { return position; }
	float3 GetRotation() const { return rotation; }
	float3 GetScale() const { return scale; }
private:

	float3 position = float3::zero;
	float3 rotation = float3::zero;
	float3 scale = float3::one;

	float4x4 localMatrix = float4x4::identity;

	Quat rotationQuat = Quat::identity;

};

