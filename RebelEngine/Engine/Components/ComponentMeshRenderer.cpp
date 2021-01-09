#include "ComponentMeshRenderer.h"

#include "Main/Application.h"

#include "CoreModules/ModuleModel.h"

ComponentMeshRenderer::ComponentMeshRenderer() {
	_type = type_component::MESHRENDERER;
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
	value[pos][JSON_TAG_UUID] = _uuid;
	value[pos][JSON_TAG_MESH_PATH] = _mesh->GetFilePath();

	return true;
}

