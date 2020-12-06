#include "ComponentMeshRenderer.h"

#include "Main/Application.h"

#include "CoreModules/ModuleModel.h"

ComponentMeshRenderer::ComponentMeshRenderer() {
	type = type_component::MESHRENDERER;
}

void ComponentMeshRenderer::Draw(){
	mesh->Draw(App->models->textures);
}
