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
	ComponentTransform(const Json::Value& value);
	ComponentTransform(const ComponentTransform& comp);
	
	~ComponentTransform() {}

	void SetTransform(const float3 position, const Quat rotation, const float3 scale);
	void SetTransform(const float3 position, const float3 rotation, const float3 scale);
	void SetTransform(const aiMatrix4x4& matrix);
	void SetTransform(const float4x4& matrix);

	void UpdateGlobalMatrix();
	void UpdateLocalMatrix();

	const float4x4& GetGlobalMatrix() const { return _globalMatrix; }
	const float4x4& GetLocalMatrix() const { return _localMatrix; }

	void DebugDraw() override;
	void OnEditor() override;

	void SetOwner(GameObject* go) override;

	float3 GetPosition() const { return _position; }
	float3 GetRotation() const { return _rotation; }
	float3 GetScale() const { return _scale; }
	Quat GetRotationQuat() const { return _rotationQuat; }

	bool ToJson(Json::Value& value, int pos) override;
	bool FromJson(const Json::Value& value) override;

private:

	void DrawDragFloat3(const char* tag, float3& vector, float speed = 0.01f);

public:

	float3 _position = float3::zero;
	float3 _rotation = float3::zero;
	float3 _scale = float3::one;

	float4x4 _localMatrix = float4x4::identity;
	float4x4 _globalMatrix = float4x4::identity;

	Quat _rotationQuat = Quat::identity;

};