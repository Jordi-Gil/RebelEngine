#include "ComponentMeshRenderer.h"

#include "Main/Application.h"

#include "CoreModules/ModuleModel.h"

ComponentMeshRenderer::ComponentMeshRenderer() {
	type = type_component::MESHRENDERER;
}

void ComponentMeshRenderer::Draw(){
	assert(owner != nullptr && "Component without and owner");
	mesh->Draw(App->models->textures, owner->GetGlobalMatrix());
}
