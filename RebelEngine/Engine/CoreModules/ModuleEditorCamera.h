#pragma once

#include "Module.h"
#include <Geometry/Frustum.h>

#define PI 3.14159265358979323846  /* pi */
#define DEGTORAD PI/180

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

public:

	ModuleEditorCamera::ModuleEditorCamera() : updated(true), movSpeed(2), rotSpeed(2) {}
	ModuleEditorCamera::~ModuleEditorCamera() {}

	bool Init();

	update_status Update();

	void WindowResized(unsigned width, unsigned height);

#pragma region setters

	void SetHorizontalFov(float fov);
	void SetVerticalFov(float fov);
	void SetAspectRatio(float aspectRatio);
	
	void SetPlaneDistances(float znear, float zfar);
	void SetZNear(float znear);
	void SetZFar(float zfar);
	
	void SetPosition(float x, float y, float z);
	void SetOrientation();

#pragma endregion setters

#pragma region getters

	void GetMatrix(matrix_type _mType, float4x4& matrix);
	void GetOpenGLMatrix(matrix_type _mType, float4x4& matrix);

	float GetHorizontalFov();
	float GetVerticalFov();
	float GetAspectRatio();

	void GetPlaneDistances(float &znear, float &zfar);
	float2 GetPlaneDistances();
	float GetZNear();
	float GetZFar();

	void GetPosition(float &x, float &y, float &z);
	float3 GetPosition() { return frustum.Pos(); }

#pragma endregion getters

private:

	Frustum frustum;
	bool updated;
	float movSpeed;
	float rotSpeed;
};