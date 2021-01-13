#include "ComponentMeshRenderer.h"

#include <iostream>
#include <fstream>

#include "Math/float3x3.h"

#include "Main/Application.h"

#include "CoreModules/ModuleModel.h"

#include "ComponentTransform.h"

#include "Utils/debugdraw.h"

ComponentMeshRenderer::ComponentMeshRenderer() {
	_type = type_component::MESHRENDERER;
}

ComponentMeshRenderer::ComponentMeshRenderer(const Json::Value& value) {
	
	Component::FromJson(value);
	_type = type_component::MESHRENDERER;
	this->FromJson(value);
}

ComponentMeshRenderer::~ComponentMeshRenderer() {
	delete _mesh;
	_mesh = nullptr;

	delete _material;
	_material = nullptr;
}

void ComponentMeshRenderer::SetMesh(Mesh* component_mesh) {
	_mesh = component_mesh;
}

void ComponentMeshRenderer::SetMaterial(Material* component_material) {
	_material = component_material;
}

void ComponentMeshRenderer::DebugDraw(){
	
	assert(_owner != nullptr && "Component without and owner");
	
	if (_owner->IsSelected()) {
		AABB box;  _owner->GetAABB(box);
		OBB obb = box.Transform(_owner->GetGlobalMatrix());
		AABB res = obb.MinimalEnclosingAABB();
		dd::aabb(res.minPoint, res.maxPoint, dd::colors::Orange);
	}
}

void ComponentMeshRenderer::Render() {
	
	//TODO: check if active & change texture to Material
	_mesh->Draw(_material, _owner->GetGlobalMatrix());
}

bool ComponentMeshRenderer::ToJson(Json::Value& value, int pos) 
{
	Component::ToJson(value, pos);
	value[pos][JSON_TAG_UUID] = _uuid;
	value[pos][JSON_TAG_MESH_PATH] = _mesh->GetFilePath();

	return true;
}

bool ComponentMeshRenderer::FromJson(const Json::Value& value) 
{
	if(!value.isNull())
	{
		std::ifstream ifs(value[JSON_TAG_MESH_PATH].asString());
		Json::CharReaderBuilder reader;
		Json::Value obj;
		std::string error;

		if (!Json::parseFromStream(reader, ifs, &obj, &error)) 
		{
			LOG(_ERROR, "Error parsing file: %s", error);
			return false;
		}

		_mesh = new Mesh();
		_mesh->FromJson(obj[0]);
		_mesh->SetFilePath(value[JSON_TAG_MESH_PATH].asCString());
	}
	else 
	{
		return false;
	}
	return true;
}

