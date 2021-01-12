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

ComponentCamera::ComponentCamera(const Json::Value& value)  {
	Component::FromJson(value);
	_type = type_component::CAMERA;
	this->FromJson(value);
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

float4x4 ComponentCamera::GetOpenGLViewMatrix() const {
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

bool ComponentCamera::ToJson(Json::Value& value, int pos) {
	Component::ToJson(value, pos);
	value[pos][JSON_TAG_BACKGROUND_TYPE] = (int)back_type;
	Json::Value jValue(Json::arrayValue);

	jValue.append(color[0]); jValue.append(color[1]); jValue.append(color[2]); jValue.append(color[3]);
	value[pos][JSON_TAG_COLOR] = jValue; jValue.clear();
	value[pos][JSON_TAG_ZNEAR] = znear;
	value[pos][JSON_TAG_ZFAR] = zfar;

	jValue.append(frustum.Pos()[0]); jValue.append(frustum.Pos()[1]); jValue.append(frustum.Pos()[2]);
	value[pos][JSON_TAG_POSITION] = jValue; jValue.clear();

	jValue.append(frustum.Front()[0]); jValue.append(frustum.Front()[1]); jValue.append(frustum.Front()[2]);
	value[pos][JSON_TAG_FRONT] = jValue; jValue.clear();

	jValue.append(frustum.Up()[0]); jValue.append(frustum.Up()[1]); jValue.append(frustum.Up()[2]);
	value[pos][JSON_TAG_UP] = jValue; jValue.clear();

	value[pos][JSON_TAG_FOV_HORIZONTAL] = frustum.HorizontalFov();
	value[pos][JSON_TAG_ASPECT_RATIO] = frustum.AspectRatio();

	return true;
}

bool ComponentCamera::FromJson(const Json::Value& value) {
	if (!value.isNull()) 
	{
		color = float4(value[JSON_TAG_COLOR][0].asFloat(), value[JSON_TAG_COLOR][1].asFloat(), value[JSON_TAG_COLOR][2].asFloat(), value[JSON_TAG_COLOR][3].asFloat());
		zfar = value[JSON_TAG_ZFAR].asFloat();
		znear = value[JSON_TAG_ZNEAR].asFloat();
		back_type = (background_type) value[JSON_TAG_ZNEAR].asInt();

		frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
		frustum.SetViewPlaneDistances(znear, zfar);
		frustum.SetHorizontalFovAndAspectRatio(value[JSON_TAG_FOV_HORIZONTAL].asFloat(), value[JSON_TAG_ASPECT_RATIO].asFloat());
		frustum.SetFront(vec(value[JSON_TAG_FRONT][0].asFloat(), value[JSON_TAG_FRONT][1].asFloat(), value[JSON_TAG_FRONT][2].asFloat()));
		frustum.SetUp(vec(value[JSON_TAG_UP][0].asFloat(), value[JSON_TAG_UP][1].asFloat(), value[JSON_TAG_UP][2].asFloat()));
		frustum.SetPos(vec(value[JSON_TAG_POSITION][0].asFloat(), value[JSON_TAG_POSITION][1].asFloat(), value[JSON_TAG_POSITION][2].asFloat()));
		
	}
	else {
		return false;
	}

	return true;
}