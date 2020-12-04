#include "ComponentMeshRenderer.h"

ComponentMeshRenderer::ComponentMeshRenderer() {
	type = type_component::MESHRENDERER;
}

void ComponentMeshRenderer::Draw(){
	mesh->Draw();
}
