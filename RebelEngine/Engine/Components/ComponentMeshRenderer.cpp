#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"

#include "Main/Application.h"

#include "CoreModules/ModuleModel.h"

#include "Utils/debugdraw.h"

#include "Math/float3x3.h"

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
	
	if (_owner->IsSelected()) {
		AABB box;  _owner->GetAABB(box);
		OBB obb = box.Transform(_owner->GetGlobalMatrix());
		AABB res = obb.MinimalEnclosingAABB();
		dd::aabb(res.minPoint, res.maxPoint, dd::colors::Orange);
	}
	_mesh->Draw(App->models->textures, _owner->GetGlobalMatrix());
}
