#pragma once
#include "Main/Mesh.h"
#include "Components/Component.h"
class ComponentMeshRenderer : public Component {

public:

	ComponentMeshRenderer();
	~ComponentMeshRenderer();

	void Enable() override {}
	void Update() override {}
	void Disable() override {}

	void SetMesh(Mesh* component_mesh);

	void Draw() override;

	type_component GetType() const override { return _type; }
	uint32_t GetMorton() const { return _mesh->GetMorton(); }
	void GetAABB(AABB& aabb) { 
		//aabb.Enclose(_mesh->_aabb); 
		aabb = _mesh->_aabb;
	};

private:

	Mesh* _mesh = nullptr;
};

