#pragma once

#include "Module.h"

#include <Components/ComponentCamera.h>

#define PI 3.14159265358979323846  /* pi */

enum class matrix_type {
	PROJECTION_MATRIX = 0, VIEW_MATRIX
};

class ModuleEditorCamera : public Module {

public:

	ModuleEditorCamera::ModuleEditorCamera() : movSpeed(2), rotSpeed(0.5), zoomSpeed(2) {}
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

	const float GetHorizontalFov() const { return camera->GetHorizontalFov(); }
	const float GetVerticalFov() const { return camera->GetVerticalFov(); }
	const float GetAspectRatio() const { return camera->GetAspectRatio(); }

	const float3 GetFront() const { return camera->GetFront(); }
	const float3 GetUp() const { return camera->GetUp(); }
	const float3 GetRight() const { return camera->GetRight(); }

	const float GetZNear() const { return camera->GetZNear(); }
	const float GetZFar() const { return camera->GetZFar(); }

	const float3 GetPosition() const { return camera->GetPosition(); }

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


	ComponentCamera* camera;

	float movSpeed;
	float zoomSpeed;
	float rotSpeed;

	float pitch = 0;

};