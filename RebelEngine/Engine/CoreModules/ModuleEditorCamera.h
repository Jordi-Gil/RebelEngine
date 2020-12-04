#pragma once

#include "Module.h"

#include <Geometry/Frustum.h>

#define PI 3.14159265358979323846  /* pi */

enum class matrix_type {
	PROJECTION_MATRIX = 0, VIEW_MATRIX
};

class ModuleEditorCamera : public Module {

public:

	ModuleEditorCamera::ModuleEditorCamera() : movSpeed(2), rotSpeed(0.5), zoomSpeed(2), currentZNear(0.1f), currentZFar(100.0f) {}
	ModuleEditorCamera::~ModuleEditorCamera() {}

	bool Init() override;

	update_status Update() override;

	void WindowResized(unsigned width, unsigned height);

	void Focus();

#pragma region setters

	void SetHorizontalFov(float hFov, float ar);
	void SetVerticalFov(float vFov, float aspectRatio);
	
	void SetZNear(float znear);
	void SetZFar(float zfar);
	
	void SetPosition(float x, float y, float z);

	void SetMovSpeed(float _speed);
	void SetRotSpeed(float _speed);
	void SetZoomSpeed(float _speed);

#pragma endregion setters

#pragma region getters

	void GetMatrix(matrix_type _mType, float4x4& matrix);
	void GetOpenGLMatrix(matrix_type _mType, float4x4& matrix);

	const float GetHorizontalFov() const { return frustum.HorizontalFov(); }
	const float GetVerticalFov() const { return frustum.VerticalFov(); }
	const float GetAspectRatio() const { return frustum.AspectRatio(); }

	const float3 GetFront() const { return frustum.Front(); }
	const float3 GetUp() const { return frustum.Up(); }
	const float3 GetRight() const { return frustum.WorldRight(); }

	const float GetZNear() const { return frustum.NearPlaneDistance(); }
	const float GetZFar() const { return frustum.FarPlaneDistance(); }

	const float3 GetPosition() const { return frustum.Pos(); }

	const float GetMovSpeed() const { return movSpeed; }
	const float GetRotSpeed() const { return rotSpeed; }
	const float GetZoomSpeed() const { return zoomSpeed; }

#pragma endregion getters

private:

	void TranslateKeyboard();
	void TranslateMouse(int x, int y);
	void TranslateMouseWheel();
	void RotateKeyboard();
	void RotateMouse(int x, int y);
	void OrbitCenterScene(int x, int y);

private:

	Frustum frustum;
	float movSpeed;
	float zoomSpeed;
	float rotSpeed;
	float currentZNear;
	float currentZFar;

	float pitch = 0;
	
};