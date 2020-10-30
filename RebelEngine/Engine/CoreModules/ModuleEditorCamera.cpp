#include "ModuleEditorCamera.h"
#include "Math/float3x3.h"
#include <cmath>

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

	return float4x4::identity;

}

update_status ModuleEditorCamera::Update() {

	return UPDATE_CONTINUE;

}

ModuleEditorCamera::~ModuleEditorCamera() {}

bool ModuleEditorCamera::Init() {

	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 200.0f);
	frustum.SetHorizontalFovAndAspectRatio((float)DEGTORAD * 90.0f, 1.3f);
	frustum.SetPos(float3(0, 5, -10));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart().RotateX((float)DEGTORAD * 45.0f);
	rotationMatrix = rotationMatrix.RotateY((float)DEGTORAD * -15.0f);
	vec oldFront = frustum.Front().Normalized();
	frustum.SetFront(rotationMatrix.MulDir(oldFront));
	vec oldUp = frustum.Up().Normalized();
	frustum.SetUp(rotationMatrix.MulDir(oldUp));

	frustum.Translate(vec{ 15,0,0 });

	projectionGL = frustum.ProjectionMatrix().Transposed(); //<-- Important to transpose!
	viewGL = frustum.ViewMatrix();
	viewGL.Transpose();

	updated = true;

	return true;
}

void ModuleEditorCamera::WindowResized(unsigned width, unsigned height) {

	LOG("Resizing the window");
	float FOV = frustum.VerticalFov();
	float aspectRatio = float(width) / height;
	frustum.SetHorizontalFovAndAspectRatio(2 * atan(tan(FOV / 2) * aspectRatio), aspectRatio);

	updateProjectMatrix();

	updated = true;

}