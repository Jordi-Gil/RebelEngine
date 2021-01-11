#include "ComponentCamera.h"

ComponentCamera::ComponentCamera() {

	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(znear, zfar);
	frustum.SetHorizontalFovAndAspectRatio(DegToRad(90), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	frustum.SetPos(float3(0, 4, 10));
	frustum.SetFront(-float3::unitZ);
	frustum.SetUp(float3::unitY);

	_type = type_component::CAMERA;
}

ComponentCamera::ComponentCamera(const Json::Value& value) 
{
	_type = type_component::CAMERA;
	this->FromJson(value);
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

bool ComponentCamera::ToJson(Json::Value& value, int pos) 
{
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

bool ComponentCamera::FromJson(const Json::Value& value)
{
	if (!value.isNull()) 
	{
		color = float4(value[JSON_TAG_COLOR][0].asFloat(), value[JSON_TAG_COLOR][1].asFloat(), value[JSON_TAG_COLOR][2].asFloat(), value[JSON_TAG_COLOR][3].asFloat());
		zfar = value[JSON_TAG_ZFAR].asFloat();
		znear = value[JSON_TAG_ZNEAR].asFloat();
		back_type = (background_type) value[JSON_TAG_ZNEAR].asInt();

		frustum.SetFront(vec(value[JSON_TAG_FRONT][0].asFloat(), value[JSON_TAG_FRONT][1].asFloat(), value[JSON_TAG_FRONT][2].asFloat()));
		frustum.SetUp(vec(value[JSON_TAG_UP][0].asFloat(), value[JSON_TAG_UP][1].asFloat(), value[JSON_TAG_UP][2].asFloat()));
		frustum.SetPos(vec(value[JSON_TAG_POSITION][0].asFloat(), value[JSON_TAG_POSITION][1].asFloat(), value[JSON_TAG_POSITION][2].asFloat()));
		frustum.SetHorizontalFovAndAspectRatio(value[JSON_TAG_FOV_HORIZONTAL].asFloat(), value[JSON_TAG_ASPECT_RATIO].asFloat());
	}
	else {

		return false;
	}

	return true;
}