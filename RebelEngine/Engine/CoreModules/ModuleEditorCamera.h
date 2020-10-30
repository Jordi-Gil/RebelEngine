#pragma once

#include "Module.h"
#include <Geometry/Frustum.h>

#define M_PI 3.14159265358979323846  /* pi */
#define DEGTORAD M_PI/180

enum matrix_type {
	PROJECTION_MATRIX, VIEW_MATRIX
};

class ModuleEditorCamera : public Module
{

public:
	ModuleEditorCamera();
	~ModuleEditorCamera();

	bool Init();

	update_status Update();

	float4x4 GetMatrix(matrix_type _mType);

	void WindowResized(unsigned width, unsigned height);

	void updateProjectMatrix() {
		projectionGL = frustum.ProjectionMatrix().Transposed();
	}

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
	float4x4 projectionGL;
	float4x4 viewGL;
	bool updated = false;
};