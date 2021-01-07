#include "ComponentCamera.h"

#include "Utils/Globals.h"

#include "Main/GameObject.h"

ComponentCamera::ComponentCamera() {

	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(znear, zfar);
	frustum.SetHorizontalFovAndAspectRatio(DegToRad(90), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	frustum.SetPos(float3(0, 4, 10));
	frustum.SetFront(-float3::unitZ);
	frustum.SetUp(float3::unitY);

	_type = type_component::CAMERA;
}

void ComponentCamera::Translate(vec _offset) {
	frustum.Translate(_offset);
}

void ComponentCamera::SetVerticalFov(float vFov, float aspectRatio) {
	frustum.SetVerticalFovAndAspectRatio(DegToRad(vFov), aspectRatio);
}

void ComponentCamera::SetHorizontalFov(float hFov, float aspectRatio) {
	frustum.SetHorizontalFovAndAspectRatio(DegToRad(hFov), aspectRatio);
}

void ComponentCamera::SetPosition(float x, float y, float z) {
	frustum.SetPos(vec(x, y, z));
}

void ComponentCamera::SetPosition(vec _pos) {
	frustum.SetPos(_pos);
}

void ComponentCamera::SetFront(vec _front){
	frustum.SetFront(_front);
}

void ComponentCamera::SetUp(vec _up) {
	frustum.SetUp(_up);
}

void ComponentCamera::SetZNear(float _znear) {
	znear = _znear;
	frustum.SetViewPlaneDistances(znear, zfar);
}

void ComponentCamera::SetZFar(float _zfar) {
	zfar = _zfar;
	frustum.SetViewPlaneDistances(znear, zfar);
}

float4x4 ComponentCamera::GetOpenGLProjectionMatrix() const
{
	float4x4 matrix = frustum.ProjectionMatrix();
	matrix.Transpose();
	return matrix;
}

float4x4 ComponentCamera::GetOpenGLViewMatrix() const
{
	float4x4 matrix = frustum.ViewMatrix();
	matrix.Transpose();
	return matrix;
}

void ComponentCamera::Draw() {
	if (_owner->IsSelected()) {
		float4x4 matrix = this->GetProjectionMatrix() * this->GetViewMatrix();
		matrix.Inverse();
		dd::frustum(matrix, dd::colors::Tomato);
	}
}