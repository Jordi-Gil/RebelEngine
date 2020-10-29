#include "ModuleEditorCamera.h"
#include <GL/glew.h>


ModuleEditorCamera::ModuleEditorCamera() {

}

float4x4 ModuleEditorCamera::GetMatrix(matrix_type _mType) {

	switch (_mType) {
		case PROJECTION_MATRIX:
			return projectionGL;
			break;
		case VIEW_MATRIX:
			return viewGL;
			break;
	}

}

update_status ModuleEditorCamera::Update() {

	return UPDATE_CONTINUE;

}

ModuleEditorCamera::~ModuleEditorCamera() {}

bool ModuleEditorCamera::Init() {

	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 200.0f);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);
	frustum.SetPos(float3(0, 1, -2));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	projectionGL = frustum.ProjectionMatrix().Transposed(); //<-- Important to transpose!
	viewGL = frustum.ViewMatrix();
	viewGL.Transpose();

	return true;
}
