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

	void GetMatrix(matrix_type _mType, float4x4& matrix);

	void WindowResized(unsigned width, unsigned height);

	bool projectionChange() const {
		return updated;
	};

	void setUpdated(bool _val) {
		updated = _val;
	};

	void SetFov();
	void SetAspectRatio();
	void SetPlaneDistances();
	void SetPosition();
	void SetOrientation();
	void LookAt(float x, float y, float z);

private:

	Frustum frustum;
	bool updated;
	float movSpeed;
	float rotSpeed;
};