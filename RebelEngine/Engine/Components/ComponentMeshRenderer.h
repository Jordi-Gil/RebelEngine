#pragma once

#include "Main/Mesh.h"

#include "Materials/Material.h"

#include "Components/Component.h"

class ComponentMeshRenderer : public Component {

public:

	ComponentMeshRenderer();
	ComponentMeshRenderer(const Json::Value& value);
	~ComponentMeshRenderer();

	void Enable() override {}
	void Update() override {}
	void Disable() override {}

	void SetMesh(Mesh* component_mesh);
	void SetMaterial(Material* component_material);

	void DebugDraw() override;
	void OnEditor() override;

	void Render();

	type_component GetType() const override { return _type; }
	uint32_t GetMorton() const { return _mesh->GetMorton(); }
	void GetAABB(AABB& aabb) { 
		//aabb.Enclose(_mesh->_aabb); 
		aabb = _mesh->_aabb;
	};
	bool ToJson(Json::Value& value, int pos);
	bool FromJson(const Json::Value& value);

private:

	Mesh* _mesh = nullptr;
	Material* _material = nullptr;

};

