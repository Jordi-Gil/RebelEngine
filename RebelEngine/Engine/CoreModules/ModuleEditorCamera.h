#pragma once

#include "Module.h"
#include <Geometry/Frustum.h>

#define PI 3.14159265358979323846  /* pi */
#define DEGTORAD PI/180
#define RADTODEG 180/PI

enum matrix_type {
	PROJECTION_MATRIX, VIEW_MATRIX
};

class ModuleEditorCamera : public Module
{
private:

	void TranslateKeyboard();
	void TranslateMouse(int x, int y);
	void TranslateMouseWheel();
	void RotateKeyboard();
	void RotateMouse(int x, int y);
	void OrbitCenterScene(int x, int y);

public:

	ModuleEditorCamera::ModuleEditorCamera() : movSpeed(2), rotSpeed(2), zoomSpeed(2), currentZNear(0.1f), currentZFar(100.0f) {}
	ModuleEditorCamera::~ModuleEditorCamera() {}

	bool Init();

	update_status Update();

	void WindowResized(unsigned width, unsigned height);

#pragma region setters

	void SetHorizontalFov(float hFov, float ar);
	void SetVerticalFov(float vFov, float aspectRatio);
	void SetAspectRatio(float aspectRatio);
	
	void SetZNear(float znear);
	void SetZFar(float zfar);
	
	void SetPosition(float x, float y, float z);
	void SetOrientation();

	void SetMovSpeed(float _speed);
	void SetRotSpeed(float _speed);
	void SetZoomSpeed(float _speed);

#pragma endregion setters

#pragma region getters

	void GetMatrix(matrix_type _mType, float4x4& matrix);
	void GetOpenGLMatrix(matrix_type _mType, float4x4& matrix);

	float GetHorizontalFov() { return frustum.HorizontalFov(); }
	float GetVerticalFov() { return frustum.VerticalFov(); }
	float GetAspectRatio() { return frustum.AspectRatio(); }

	float3 GetFront() { return frustum.Front(); }
	float3 GetUp() { return frustum.Up(); }
	float3 GetRight() { return frustum.WorldRight(); }

	float GetZNear() { return frustum.NearPlaneDistance(); }
	float GetZFar() { return frustum.FarPlaneDistance(); }

	float3 GetPosition() { return frustum.Pos(); }

	float GetMovSpeed() { return movSpeed; }
	float GetRotSpeed() { return rotSpeed; }
	float GetZoomSpeed() { return zoomSpeed; }

#pragma endregion getters

	float4 clearColor = float4(0.1f, 0.1f, 0.1f, 1.0f);

private:

	Frustum frustum;
	float movSpeed;
	float zoomSpeed;
	float rotSpeed;
	float currentZNear;
	float currentZFar;
	
};