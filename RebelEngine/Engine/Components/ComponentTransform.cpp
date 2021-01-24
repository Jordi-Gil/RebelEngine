#include "ComponentTransform.h"

#include <assimp/cimport.h>

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGui/imgui_utils.h>

#include "Math/float3x3.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Utils/debugdraw.h"

#include "CoreModules/ModuleScene.h"

#include "ComponentCamera.h"

#include "AccelerationDataStructures/Octree.h"

static constexpr char* FLOAT3_LABELS[3] = { {"X"},{"Y"},{"Z"} };

ComponentTransform::ComponentTransform() {
	_type = ComponentType::kTRANSFORM;
}

ComponentTransform::ComponentTransform(const ComponentTransform& comp) {

	_owner = comp._owner;
	_active = comp._active;
	_type = comp._type;
	_uuid = comp._uuid;
	_position = comp._position;
	_rotation = comp._rotation;
	_scale = comp._scale;
	_rotationQuat = comp._rotationQuat;
	_localMatrix = comp._localMatrix;
	_globalMatrix = comp._globalMatrix;

}

ComponentTransform::ComponentTransform(const float3 position, const float3 rotation, const float3 scale) {

	_type = ComponentType::kTRANSFORM;

	_position = position;
	_rotation = rotation;
	_scale = scale;

	_rotationQuat = Quat::FromEulerXYZ(DegToRad(rotation.x), DegToRad(rotation.y), DegToRad(rotation.z));

	_localMatrix = float4x4::FromTRS(_position, _rotationQuat, _scale);
}

ComponentTransform::ComponentTransform(const aiMatrix4x4& matrix) {

	_type = ComponentType::kTRANSFORM;

	aiVector3D pos, sca;
	aiQuaternion rot;

	aiDecomposeMatrix(&matrix, &sca, &rot, &pos);

	_position = float3(pos.x, pos.y, pos.z);
	_rotation = float3(matrix.b1, matrix.b2, matrix.b3);
	_scale = float3(sca.x, sca.y, sca.z);

	_rotationQuat = Quat(rot.x, rot.y, rot.z, rot.w);

	_localMatrix = float4x4::FromTRS(_position, _rotationQuat, _scale);

}

void ComponentTransform::SetTransform(const float3 position, const Quat rotation, const float3 scale) {

	_position = position;
	_rotation = rotation.ToEulerXYZ();
	_scale = scale;

	_rotationQuat = rotation;

	_localMatrix = float4x4::FromTRS(_position, _rotationQuat, _scale);

}

ComponentTransform::ComponentTransform(const Json::Value& value) {

	Component::FromJson(value);
	_type = ComponentType::kTRANSFORM;
	FromJson(value);
}

void ComponentTransform::SetTransform(const float3 position, const float3 rotation, const float3 scale) {
	
	_position = position;
	_rotation = rotation;
	_scale = scale;

	_rotationQuat = Quat::FromEulerXYZ(DegToRad(rotation.x), DegToRad(rotation.y), DegToRad(rotation.z));

	_localMatrix = float4x4::FromTRS(_position, _rotationQuat, _scale);

	UpdateGlobalMatrix();

}

void ComponentTransform::SetTransform(const float4x4& matrix) {

	float3x3 rot = matrix.RotatePart();
	float4x4 aux;
	matrix.Decompose(_position, aux, _scale);
	float3 rad = aux.ToEulerXYZ();
	_rotation = float3(RadToDeg(rad.x), RadToDeg(rad.y), RadToDeg(rad.z));

	_rotationQuat = Quat(aux);

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

void ComponentTransform::UpdateLocalMatrix(){
	
	_rotationQuat = Quat::FromEulerXYZ(DegToRad(_rotation.x), DegToRad(_rotation.y), DegToRad(_rotation.z));

	_localMatrix = float4x4::FromTRS(_position, _rotationQuat, _scale);

	UpdateGlobalMatrix();

}

void ComponentTransform::UpdateGlobalMatrix() {
	
	if (_owner != nullptr) {
		GameObject* parent = _owner->GetParent();
		if (parent != nullptr) {
			_globalMatrix = parent->GetGlobalMatrix() * _localMatrix;
		}
		else
			_globalMatrix = _localMatrix;
		
		if(App->scene->_octree != nullptr && (_owner->GetMask() & GO_MASK_MESH) != 0) 
			App->scene->_octree->Update(App->scene->_octree->_root, _owner);
	}
}

void ComponentTransform::DebugDraw() {
	if (_owner->IsSelected()) { 
		//dd::axisTriad(_globalMatrix, 0.1f, 2.0f); 
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

bool ComponentTransform::FromJson(const Json::Value& value) {

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

void ComponentTransform::DrawDragFloat3(const char* tag, float3& vector, float speed) {

	char invis[FILENAME_MAX] = "##";
	strcat_s(invis, tag);
	ImGui::BeginColumns(invis, 4, ImGuiColumnsFlags_NoBorder | ImGuiColumnsFlags_NoResize);
	{
		ImGui::Text(tag);
		for (int i = 0; i < 3; ++i) {
			ImGui::NextColumn();
			ImGui::PushID(&vector[i]);
			ImGui::Text(FLOAT3_LABELS[i]);
			ImGui::SameLine();
			if (ImGui::DragFloat("", &vector[i], speed)) {
				UpdateLocalMatrix();
				_owner->UpdateChildrenTransform();
			};
			ImGui::PopID();
		}
	}
	ImGui::EndColumns();
}

void ComponentTransform::OnEditor() {

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Text(_owner->GetName());
		ImGui::Separator();

		DrawDragFloat3("Position", _position);
		DrawDragFloat3("Rotation", _rotation);
		DrawDragFloat3("Scale", _scale);

		if ((_owner->GetMask() & GO_MASK_CAMERA) != 0) {
			ComponentCamera* cam = (ComponentCamera*) _owner->GetComponent(ComponentType::kCAMERA);
			cam->Transform(_rotationQuat, _position);
		}

	}
}
