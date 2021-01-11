#include "ComponentCamera.h"

#include "Utils/Globals.h"

#include "Main/GameObject.h"

#include "Geometry/Plane.h"

ComponentCamera::ComponentCamera() {

	_frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	_frustum.SetViewPlaneDistances(_znear, _zfar);
	_frustum.SetHorizontalFovAndAspectRatio(DegToRad(90), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	_frustum.SetPos(float3(0, 4, 10));
	_frustum.SetFront(-float3::unitZ);
	_frustum.SetUp(float3::unitY);

	_type = type_component::CAMERA;

	GenerateFrustumPlanes();

}

Plane GetPlane(const float4& vec) {

	float3 normal = vec.xyz();
	assert(normal.Normalize() != 0);
	float d = vec.w / normal.Length();
	return Plane(normal, d);
}

void ComponentCamera::GenerateFrustumPlanes() {

	_planes.clear(); _planes.shrink_to_fit();

	float4x4 matrix = _frustum.ViewMatrix() * _frustum.ProjectionMatrix();

	_planes.push_back(GetPlane(matrix.Col(3) - matrix.Col(0)));
	_planes.push_back(GetPlane(matrix.Col(3) + matrix.Col(0)));
	_planes.push_back(GetPlane(matrix.Col(3) + matrix.Col(1)));
	_planes.push_back(GetPlane(matrix.Col(3) - matrix.Col(1)));
	_planes.push_back(GetPlane(matrix.Col(3) - matrix.Col(2)));
	_planes.push_back(GetPlane(matrix.Col(3) + matrix.Col(2)));

	_dity_planes = false;
}

void ComponentCamera::Translate(vec offset) {
	_frustum.Translate(offset);

	GenerateFrustumPlanes();
}

void ComponentCamera::SetVerticalFov(float vFov, float aspectRatio) {
	_frustum.SetVerticalFovAndAspectRatio(DegToRad(vFov), aspectRatio);
}

void ComponentCamera::SetHorizontalFov(float hFov, float aspectRatio) {
	_frustum.SetHorizontalFovAndAspectRatio(DegToRad(hFov), aspectRatio);
}

void ComponentCamera::SetPosition(float x, float y, float z) {
	_frustum.SetPos(vec(x, y, z));
	_dity_planes = true;
}

void ComponentCamera::SetPosition(vec pos) {
	_frustum.SetPos(pos);
	_dity_planes = true;
}

void ComponentCamera::SetFront(vec front){
	_frustum.SetFront(front);
	_dity_planes = true;
}

void ComponentCamera::SetUp(vec up) {
	_frustum.SetUp(up);
	_dity_planes = true;
}

void ComponentCamera::SetZNear(float znear) {
	_znear = znear;
	_frustum.SetViewPlaneDistances(_znear, _zfar);
	_dity_planes = true;
}

void ComponentCamera::SetZFar(float zfar) {
	_zfar = zfar;
	_frustum.SetViewPlaneDistances(_znear, _zfar);
	_dity_planes = true;
}

float4x4 ComponentCamera::GetOpenGLProjectionMatrix() const {
	float4x4 matrix = _frustum.ProjectionMatrix();
	matrix.Transpose();
	return matrix;
}

float4x4 ComponentCamera::GetOpenGLViewMatrix() const
{
	float4x4 matrix = _frustum.ViewMatrix();
	matrix.Transpose();
	return matrix;
}

//TODO: Fix
float ComponentCamera::Intersects(AABB& box, Plane& plane) {

	float r = fabsf(box.Size().x * plane.normal.x)
		    + fabsf(box.Size().y * plane.normal.y)
		    + fabsf(box.Size().z * plane.normal.z);

	//Signed distance between box center and plane
	float d = plane.normal.Dot(box.CenterPoint()) + plane.d;

	if (fabsf(d) < r) { return 0; }
	else if (d < 0.0f) { return d + r; }
	return d - r;
}

bool ComponentCamera::Intersects(const AABB& box) {

	return _frustum.Intersects(box);

	//assert(_planes.size() == 6 && "More/Less than 6 frustum planes");

	//if(_dity_planes) GenerateFrustumPlanes();
	
	//for (int i = 0; i < _planes.size(); i++) {
	//	if (!box.Intersects(_planes[i])) return false;
	//	//float d = Intersects(box, _planes[i]);
	//	//if (d < 0) return false;
	//}
	
	return true;

}

void ComponentCamera::Draw() {
	float4x4 matrix = this->GetProjectionMatrix() * this->GetViewMatrix();
	matrix.Inverse();
	dd::frustum(matrix, dd::colors::Tomato);
}