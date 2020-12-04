#pragma once
#include "Main/Mesh.h"
#include "Components/Component.h"
class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer();
	~ComponentMeshRenderer() {
		//delete mesh;
		//mesh = nullptr;
	}; 

	void Enable() override {}
	void Update() override {}
	void Disable() override {}

	void SetMesh(Mesh* component_mesh) { mesh = component_mesh; }

	void Draw();

	type_component GetType() const override { return type; }

private:

	Mesh* mesh = nullptr;
};
