#pragma once

#include "Component.h"

#include "Math/float4x4.h"
#include "Math/Quat.h"

#include <assimp/matrix4x4.h>

class ComponentTransform : public Component {

public:

	ComponentTransform();
	ComponentTransform(const float3 position, const float3 rotation, const float3 scale);
	ComponentTransform(const aiMatrix4x4& matrix);
	
	~ComponentTransform() {}

	void SetTransform(const float3 position, const float3 rotation, const float3 scale);
	void SetTransform(const aiMatrix4x4& matrix);

	void UpdateGlobalMatrix();

	float4x4 GetGlobalMatrix() const { return _globalMatrix; }

	void Draw();

	void SetOwner(GameObject* go) override;

	float3 GetPosition() const { return _position; }
	float3 GetRotation() const { return _rotation; }
	float3 GetScale() const { return _scale; }

	bool ToJson(Json::Value& value, int pos);

public:

	float3 _position = float3::zero;
	float3 _rotation = float3::zero;
	float3 _scale = float3::one;

	float4x4 _localMatrix = float4x4::identity;
	float4x4 _globalMatrix = float4x4::identity;

	Quat _rotationQuat = Quat::identity;

};