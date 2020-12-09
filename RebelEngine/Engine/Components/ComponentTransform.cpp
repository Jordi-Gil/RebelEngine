#include "ComponentTransform.h"

#include "Main/GameObject.h"

#include "Utils/debugdraw.h"

#include <assimp/cimport.h>

ComponentTransform::ComponentTransform() {
	type = type_component::TRANSFORM;
}

ComponentTransform::ComponentTransform(const float3 _position, const float3 _rotation, const float3 _scale) {

	type = type_component::TRANSFORM;

	position = _position;
	rotation = _rotation;
	scale = _scale;

	rotationQuat = Quat::FromEulerXYZ(DegToRad(_rotation.x), DegToRad(_rotation.y), DegToRad(_rotation.z));

	localMatrix = float4x4::FromTRS(position, rotationQuat, scale);

}

ComponentTransform::ComponentTransform(const aiMatrix4x4& matrix) {

	type = type_component::TRANSFORM;

	aiVector3D pos, sca;
	aiQuaternion rot;

	aiDecomposeMatrix(&matrix, &sca, &rot, &pos);

	position = float3(pos.x, pos.y, pos.z);
	rotation = float3(matrix.b1, matrix.b2, matrix.b3);
	scale = float3(sca.x, sca.y, sca.z);

	rotationQuat = Quat(rot.x, rot.y, rot.z, rot.w);

	localMatrix = float4x4::FromTRS(position, rotationQuat, scale);

}

void ComponentTransform::SetTransform(const float3 _position, const float3 _rotation, const float3 _scale) {
	
	position = _position;
	rotation = _rotation;
	scale = _scale;

	rotationQuat = Quat::FromEulerXYZ(DegToRad(_rotation.x), DegToRad(_rotation.y), DegToRad(_rotation.z));

	localMatrix = float4x4::FromTRS(position, rotationQuat, scale);

	UpdateGlobalMatrix();
}

void ComponentTransform::SetTransform(const aiMatrix4x4& matrix) {

	aiVector3D pos, sca;
	aiQuaternion rot;

	aiDecomposeMatrix(&matrix, &sca, &rot, &pos);

	position = float3(pos.x, pos.y, pos.z);
	rotation = float3(matrix.b1, matrix.b2, matrix.b3);
	scale = float3(sca.x, sca.y, sca.z);

	rotationQuat = Quat(rot.x, rot.y, rot.z, rot.w);

	localMatrix = float4x4::FromTRS(position, rotationQuat, scale);

	UpdateGlobalMatrix();
}

void ComponentTransform::UpdateTransform(const ComponentTransform& old_father, const ComponentTransform& new_father) {

	position = position.Sub(old_father.GetPosition()).Add(new_father.GetPosition());
	rotation = rotation.Sub(old_father.GetRotation()).Add(new_father.GetRotation());
	scale = scale.Sub(old_father.GetScale()).Add(new_father.GetScale());

	rotationQuat = Quat::FromEulerXYZ(DegToRad(position.x), DegToRad(rotation.y), DegToRad(scale.z));

	localMatrix = float4x4::FromTRS(position, rotationQuat, scale);
	
}

void ComponentTransform::UpdateGlobalMatrix() {
	
	if (owner != nullptr) {
		if (owner->GetParent() != nullptr)
			globalMatrix = owner->GetParent()->GetGlobalMatrix() * localMatrix;
		else
			globalMatrix = localMatrix;
	}
}

void ComponentTransform::Draw() {
	dd::axisTriad(localMatrix, 0.1f, 0.1f);
}

void ComponentTransform::SetOwner(GameObject* go) {
	Component::SetOwner(go);
	UpdateGlobalMatrix();
}