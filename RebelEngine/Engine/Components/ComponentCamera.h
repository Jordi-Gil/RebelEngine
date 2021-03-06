#pragma once

#include "Component.h"

#include "Utils/debugdraw.h"

#include "Geometry/LineSegment.h"
#include "Geometry/Frustum.h"
#include "Geometry/Plane.h"

enum class background_type {
	COLOR,
	SKYBOX
};

class ComponentCamera : public Component {

public:

	ComponentCamera();
	ComponentCamera(const Json::Value& value);
	ComponentCamera(const ComponentCamera& comp);

	void Translate(vec offset);
	void ToggleMainCamera();

	void SetHorizontalFov(float hFov, float ar);
	void SetVerticalFov(float vFov, float aspectRatio);

	void SetZNear(float znear);
	void SetZFar(float zfar);

	void SetPosition(float x, float y, float z);
	void SetPosition(vec pos);
	void SetFront(vec front);
	void SetUp(vec up);

	void Transform(const Quat& rotationQuat, const float3& translation);

	const float GetHorizontalFov() const { return _frustum.HorizontalFov(); }
	const float GetVerticalFov() const { return _frustum.VerticalFov(); }
	const float GetAspectRatio() const { return _frustum.AspectRatio(); }

	const float3 GetFront() const { return _frustum.Front(); }
	const float3 GetUp() const { return _frustum.Up(); }
	const float3 GetRight() const { return _frustum.WorldRight(); }

	const float GetZNear() const { return _frustum.NearPlaneDistance(); }
	const float GetZFar() const { return _frustum.FarPlaneDistance(); }

	const float3 GetPosition() const { return _frustum.Pos(); }

	float4x4 GetProjectionMatrix() const { return _frustum.ProjectionMatrix(); }
	float4x4 GetViewMatrix() const { return _frustum.ViewMatrix(); }

	float4x4 GetOpenGLProjectionMatrix() const;
	float4x4 GetOpenGLViewMatrix() const;

	bool IsMainCamera() const { return _is_main_camera; }

	bool Intersects(const AABB& box);
	float Intersects(AABB& box, Plane& plane);

	void DebugDraw() override;
	void OnEditor() override {}
	
	bool ToJson(Json::Value& value, int pos) override;
	bool FromJson(const Json::Value& value) override;

	LineSegment GetRay(float normalized_x, float normalized_y);

private:

	void GenerateFrustumPlanes();

private:

	Frustum _frustum;
	background_type _back_type = background_type::COLOR;
	float4 _color = float4(0.1f, 0.1f, 0.1f, 1.0f);

	float _znear = 10.0f;
	float _zfar = 100.0f;

	bool _dirty_planes = true;
	bool _is_main_camera = false;

	std::vector<Plane> _planes;

};