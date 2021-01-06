#pragma once

#include "Component.h"

#include "Utils/debugdraw.h"

#include <Geometry/Frustum.h>

enum class background_type {
	COLOR,
	SKYBOX
};

class ComponentCamera : public Component {

public:

	ComponentCamera();

	void Translate(vec _offset);

	void SetHorizontalFov(float hFov, float ar);
	void SetVerticalFov(float vFov, float aspectRatio);

	void SetZNear(float znear);
	void SetZFar(float zfar);

	void SetPosition(float x, float y, float z);
	void SetPosition(vec _pos);

	void SetFront(vec _front);
	void SetUp(vec _up);

	const float GetHorizontalFov() const { return frustum.HorizontalFov(); }
	const float GetVerticalFov() const { return frustum.VerticalFov(); }
	const float GetAspectRatio() const { return frustum.AspectRatio(); }

	const float3 GetFront() const { return frustum.Front(); }
	const float3 GetUp() const { return frustum.Up(); }
	const float3 GetRight() const { return frustum.WorldRight(); }

	const float GetZNear() const { return frustum.NearPlaneDistance(); }
	const float GetZFar() const { return frustum.FarPlaneDistance(); }

	const float3 GetPosition() const { return frustum.Pos(); }

	float4x4 GetProjectionMatrix() { return frustum.ProjectionMatrix(); }
	float4x4 GetViewMatrix() { return frustum.ViewMatrix(); }

	void Draw() {
		dd::frustum(frustum.ProjectionMatrix().Inverted(), dd::colors::Tomato);
	}
	
	bool ToJson(Json::Value& value, int pos);

private:

	Frustum frustum;
	background_type back_type = background_type::COLOR;
	float4 color = float4(0.1f, 0.1f, 0.1f, 1.0f);

	float znear = 0.01f;
	float zfar = 1000.0f;
};