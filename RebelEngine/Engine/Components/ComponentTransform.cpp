#include "ComponentTransform.h"

Transform::Transform(const float3 _position, const float3 _rotation, const float3 _scale) {

	type = type_component::TRANSFORM;

	position = _position;
	rotation = _rotation;
	scale = _scale;

	rotationQuat = Quat::FromEulerXYZ(DegToRad(_rotation.x), DegToRad(_rotation.y), DegToRad(_rotation.z));

	localMatrix = float4x4::FromTRS(position, rotationQuat, scale);

}

void Transform::SetTransform(const float3 _position, const float3 _rotation, const float3 _scale) {

	if (owner) {

		position = _position;
		rotation = _rotation;
		scale = _scale;

		rotationQuat = Quat::FromEulerXYZ(DegToRad(_rotation.x), DegToRad(_rotation.y), DegToRad(_rotation.z));

		localMatrix = float4x4::FromTRS(position, rotationQuat, scale);
	}
}

