#include "ComponentMeshRenderer.h"

#include "Main/Application.h"

#include "CoreModules/ModuleModel.h"

#include <iostream>
#include <fstream>

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
}

void ComponentMeshRenderer::SetMesh(Mesh* component_mesh) {
	_mesh = component_mesh;
}

void ComponentMeshRenderer::Draw(){
	assert(_owner != nullptr && "Component without and owner");
	_mesh->Draw(App->models->textures, _owner->GetGlobalMatrix());
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

