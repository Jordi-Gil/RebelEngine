#include "ComponentTransform.h"

ComponentTransform::ComponentTransform(const float3 _position, const float3 _rotation, const float3 _scale) {

	type = type_component::TRANSFORM;

	position = _position;
	rotation = _rotation;
	scale = _scale;

	rotationQuat = Quat::FromEulerXYZ(DegToRad(_rotation.x), DegToRad(_rotation.y), DegToRad(_rotation.z));

	localMatrix = float4x4::FromTRS(position, rotationQuat, scale);

}

void ComponentTransform::SetTransform(const float3 _position, const float3 _rotation, const float3 _scale) {

	if (owner) {

		position = _position;
		rotation = _rotation;
		scale = _scale;

		rotationQuat = Quat::FromEulerXYZ(DegToRad(_rotation.x), DegToRad(_rotation.y), DegToRad(_rotation.z));

		localMatrix = float4x4::FromTRS(position, rotationQuat, scale);
	}
}
void ComponentTransform::SetTransform(const aiMatrix4x4& matrix) {

	//if (owner) {
		position = float3(matrix.a1, matrix.a2, matrix.a3);
		rotation =  float3(matrix.b1, matrix.b2, matrix.b3);
		scale =  float3(matrix.c1, matrix.c2, matrix.c3);

		rotationQuat = Quat::FromEulerXYZ(DegToRad(position.x), DegToRad(rotation.y), DegToRad(scale.z));

		localMatrix = float4x4::FromTRS(position, rotationQuat, scale);
	//}
}
void ComponentTransform::UpdateTransform(const ComponentTransform& old_father, const ComponentTransform& new_father) {

	//if (owner) {
	position = position.Sub(old_father.GetPosition()).Add(new_father.GetPosition());
	rotation = rotation.Sub(old_father.GetRotation()).Add(new_father.GetRotation());
	scale = scale.Sub(old_father.GetScale()).Add(new_father.GetScale());

	rotationQuat = Quat::FromEulerXYZ(DegToRad(position.x), DegToRad(rotation.y), DegToRad(scale.z));

	localMatrix = float4x4::FromTRS(position, rotationQuat, scale);
	//}
}
