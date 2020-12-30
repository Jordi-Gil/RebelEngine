#include "ComponentMeshRenderer.h"

#include "Main/Application.h"

#include "CoreModules/ModuleModel.h"

ComponentMeshRenderer::ComponentMeshRenderer() {
	type = type_component::MESHRENDERER;
}

ComponentMeshRenderer::~ComponentMeshRenderer() {
	delete _mesh;
	_mesh = nullptr;
}

void ComponentMeshRenderer::SetMesh(Mesh* component_mesh) {
	_mesh = component_mesh;
}

void ComponentMeshRenderer::Draw(){
	assert(owner != nullptr && "Component without and owner");
	_mesh->Draw(App->models->textures, owner->GetGlobalMatrix());
}
