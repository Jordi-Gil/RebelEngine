#include "ComponentTransform.h"

#include "Main/Application.h"

#include "Main/GameObject.h"

#include "Utils/debugdraw.h"

#include "CoreModules/ModuleScene.h"

#include "AccelerationDataStructures/Octree.h"

#include <assimp/cimport.h>

ComponentTransform::ComponentTransform() {
	_type = type_component::TRANSFORM;
}

ComponentTransform::ComponentTransform(const float3 position, const float3 rotation, const float3 scale) {

	_type = type_component::TRANSFORM;

	_position = position;
	_rotation = rotation;
	_scale = scale;

	_rotationQuat = Quat::FromEulerXYZ(DegToRad(rotation.x), DegToRad(rotation.y), DegToRad(rotation.z));

	_localMatrix = float4x4::FromTRS(_position, _rotationQuat, _scale);

	UpdateGlobalMatrix();
}

ComponentTransform::ComponentTransform(const aiMatrix4x4& matrix) {

	_type = type_component::TRANSFORM;

	aiVector3D pos, sca;
	aiQuaternion rot;

	aiDecomposeMatrix(&matrix, &sca, &rot, &pos);

	_position = float3(pos.x, pos.y, pos.z);
	_rotation = float3(matrix.b1, matrix.b2, matrix.b3);
	_scale = float3(sca.x, sca.y, sca.z);

	_rotationQuat = Quat(rot.x, rot.y, rot.z, rot.w);

	_localMatrix = float4x4::FromTRS(_position, _rotationQuat, _scale);

	UpdateGlobalMatrix();

}

void ComponentTransform::SetTransform(const float3 position, const Quat rotation, const float3 scale) {

	_position = position;
	_rotation = rotation.ToEulerXYZ();
	_scale = scale;

	_rotationQuat = rotation;

	_localMatrix = float4x4::FromTRS(_position, _rotationQuat, _scale);

	UpdateGlobalMatrix();

}

void ComponentTransform::SetTransform(const float3 position, const float3 rotation, const float3 scale) {
	
	_position = position;
	_rotation = rotation;
	_scale = scale;

	_rotationQuat = Quat::FromEulerXYZ(DegToRad(rotation.x), DegToRad(rotation.y), DegToRad(rotation.z));

	_localMatrix = float4x4::FromTRS(_position, _rotationQuat, _scale);

	UpdateGlobalMatrix();

}

void ComponentTransform::SetTransform(const aiMatrix4x4& matrix) {

	aiVector3D pos, sca;
	aiQuaternion rot;

	aiDecomposeMatrix(&matrix, &sca, &rot, &pos);

	_position = float3(pos.x, pos.y, pos.z);
	_rotation = float3(matrix.b1, matrix.b2, matrix.b3);
	_scale = float3(sca.x, sca.y, sca.z);

	_rotationQuat = Quat(rot.x, rot.y, rot.z, rot.w);

	_localMatrix = float4x4::FromTRS(_position, _rotationQuat, _scale);

	UpdateGlobalMatrix();
}

void ComponentTransform::UpdateGlobalMatrix() {
	
	if (_owner != nullptr) {
		if (_owner->GetParent() != nullptr) {
			_globalMatrix = _owner->GetParent()->GetGlobalMatrix() * _localMatrix;
		}
		else
			_globalMatrix = _localMatrix;
		
		//if(App->scene->_octree != nullptr && (_owner->GetMask() & GO_MASK_MESH) != 0) 
		//	App->scene->_octree->Update(App->scene->_octree->_root, _owner);
	}
}

void ComponentTransform::Draw() {
	if (_owner->IsSelected()) { 
		dd::axisTriad(_globalMatrix, 0.1f, 2.0f); 
	}
}

void ComponentTransform::SetOwner(GameObject* go) {
	Component::SetOwner(go);
	UpdateGlobalMatrix();
}