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

	void Draw();

	type_component GetType() const override { return _type; }
	uint32_t GetMorton() const { return _mesh->GetMorton(); }
	bool ToJson(Json::Value& value, int pos);

private:
	std::string _meshPath;
	Mesh* _mesh = nullptr;
};

