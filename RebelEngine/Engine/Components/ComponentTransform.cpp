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

ComponentTransform::ComponentTransform(const Json::Value& value) 
{
	Component::FromJson(value);
	_type = type_component::TRANSFORM;
	this->FromJson(value);
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

bool ComponentTransform::ToJson(Json::Value& value, int pos) {

	Component::ToJson(value, pos);
	Json::Value jValue(Json::arrayValue);

	jValue.append(_position[0]); jValue.append(_position[1]); jValue.append(_position[2]);
	value[pos][JSON_TAG_POSITION] = jValue;	jValue.clear();

	jValue.append(_rotation[0]); jValue.append(_rotation[1]); jValue.append(_rotation[2]);
	value[pos][JSON_TAG_ROTATION] = jValue; jValue.clear();

	jValue.append(_scale[0]); jValue.append(_scale[1]); jValue.append(_scale[2]);
	value[pos][JSON_TAG_SCALE] = jValue; jValue.clear();

	return true;

}

bool ComponentTransform::FromJson(const Json::Value& value) 
{

	if(!value.isNull())
	{
		_position = float3(value[JSON_TAG_POSITION][0].asFloat(), value[JSON_TAG_POSITION][1].asFloat(), value[JSON_TAG_POSITION][2].asFloat());
		_rotation = float3(value[JSON_TAG_ROTATION][0].asFloat(), value[JSON_TAG_ROTATION][1].asFloat(), value[JSON_TAG_ROTATION][2].asFloat());
		_scale = float3(value[JSON_TAG_SCALE][0].asFloat(), value[JSON_TAG_SCALE][1].asFloat(), value[JSON_TAG_SCALE][2].asFloat());

		_rotationQuat = Quat::FromEulerXYZ(DegToRad(_rotation.x), DegToRad(_rotation.y), DegToRad(_rotation.z));
		_localMatrix = float4x4::FromTRS(_position, _rotationQuat, _scale);
	}
	else 
	{
		//TODO:JSON ERROR
		return false;
	}
	return true;
}