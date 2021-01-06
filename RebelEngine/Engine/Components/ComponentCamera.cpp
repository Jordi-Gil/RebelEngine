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
	//TODO: SE GUARDA FRUSTRUM?
	Component::ToJson(value, pos);
	value[pos][JSON_TAG_BACKGROUND_TYPE] = (int)back_type;
	value[pos][JSON_TAG_COLOR] = color.ToString();
	value[pos][JSON_TAG_ZNEAR] = znear;
	value[pos][JSON_TAG_ZFAR] = zfar;

	return true;
}