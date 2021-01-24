#pragma once

#include "Main/Mesh.h"

#include "Materials/Material.h"

#include "Materials/MAtStandard.h"

#include "Components/Component.h"

class ComponentMeshRenderer : public Component {

public:

	ComponentMeshRenderer();
	ComponentMeshRenderer(const Json::Value& value);
	ComponentMeshRenderer(const ComponentMeshRenderer& comp);
	~ComponentMeshRenderer();

	void Enable() override {}
	void Update() override {}
	void Disable() override {}

	void SetMesh(Mesh* component_mesh);
	void SetMaterial(Material* component_material);

	void DebugDraw() override;
	void OnEditor() override;

	void Render();

	ComponentType GetType() const override { return _type; }
	uint32_t GetMorton() const { return _mesh->GetMorton(); }
	void GetAABB(AABB& aabb) {  aabb = _mesh->_aabb; }

	bool ToJson(Json::Value& value, int pos) override;
	bool FromJson(const Json::Value& value) override;

public:

	//TODO: Conver to unique ptr
	Mesh* _mesh = nullptr;
	Material* _material = nullptr;

private:

	void DrawFrame(int frameId, unsigned int texId, MatStandard_Flags_ flag);

};

