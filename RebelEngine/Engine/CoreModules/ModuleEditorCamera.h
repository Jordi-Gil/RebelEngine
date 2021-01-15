#pragma once

#include "Module.h"

#include <Components/ComponentCamera.h>

#define PI 3.14159265358979323846  /* pi */

enum class matrix_type {
	PROJECTION_MATRIX = 0, VIEW_MATRIX
};

class ModuleEditorCamera : public Module {

public:

	ModuleEditorCamera() : movSpeed(2), rotSpeed(0.5), zoomSpeed(2) {}
	~ModuleEditorCamera();

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

	void SetCamera(ComponentCamera* ccamera) { camera = ccamera; }

#pragma endregion setters
	

#pragma region getters

	void GetMatrix(matrix_type _mType, float4x4& matrix);
	void GetOpenGLMatrix(matrix_type _mType, float4x4& matrix);

	float GetHorizontalFov() const { return camera->GetHorizontalFov(); }
	float GetVerticalFov() const { return camera->GetVerticalFov(); }
	float GetAspectRatio() const { return camera->GetAspectRatio(); }

	const float3 GetFront() const { return camera->GetFront(); }
	const float3 GetUp() const { return camera->GetUp(); }
	const float3 GetRight() const { return camera->GetRight(); }

	float GetZNear() const { return camera->GetZNear(); }
	float GetZFar() const { return camera->GetZFar(); }

	const float3 GetPosition() const { return camera->GetPosition(); }

	float GetMovSpeed() const { return movSpeed; }
	float GetRotSpeed() const { return rotSpeed; }
	float GetZoomSpeed() const { return zoomSpeed; }
	ComponentCamera* GetCamera() const { return camera; }

#pragma endregion getters

private:

	void TranslateKeyboard();
	void TranslateMouse(int x, int y);
	void TranslateMouseWheel();
	void RotateKeyboard();
	void RotateMouse(int x, int y);
	void OrbitCenterScene(int x, int y);
	bool GetObjectPicked();
	GameObject* GetObjectPickedRec(LineSegment& ray, bool& hit, float& minDistance, GameObject& father);

private:

	ComponentCamera* camera = nullptr;

	float movSpeed;
	float zoomSpeed;
	float rotSpeed;

	float pitch = 0.0f;
};