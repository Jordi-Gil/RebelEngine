#pragma once
#include "Main/Mesh.h"
#include "Components/Component.h"
class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer();
	~ComponentMeshRenderer() {}; // TODO: hacer delete del mesh

	void Enable() {}
	void Update() {}
	void Disable() {}

	void SetMesh(Mesh* component_mesh) { mesh = component_mesh; }
private:
	Mesh* mesh = nullptr;
};

