#include "ComponentMeshRenderer.h"

#include <iostream>
#include <fstream>

#include "Math/float3x3.h"
#include "Math/float2.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/IconsFontAwesome5.h"
#include "ImGui/IconsForkAwesome.h"

#include "Main/Application.h"
#include "CoreModules/ModuleGUI.h"
#include "CoreModules/ModuleModel.h"
#include "CoreModules/ModuleTexture.h"
#include "ComponentTransform.h"
#include "Materials/MatStandard.h"
#include "GUIs/GUIProject.h"
#include "Utils/debugdraw.h"

//Actually only one, but in a future there will be more
static const char* shaderTypes[1] = {"Standard (Specular)"};

bool IsImage(const std::string& extension) {

	for (int i = 0; i < MARRAYSIZE(imageFormat); i++) {
		if (_strcmpi(imageFormat[i], extension.c_str()) == 0 ) return true;
	}
	return false;
}

ComponentMeshRenderer::ComponentMeshRenderer() {
	_type = type_component::MESHRENDERER;
}

ComponentMeshRenderer::ComponentMeshRenderer(const ComponentMeshRenderer& comp) {
	this->_owner = comp._owner;
	this->_active = comp._active;
	this->_type = comp._type;
	this->_uuid = comp._uuid;
	this->_mesh = comp._mesh;
}

ComponentMeshRenderer::ComponentMeshRenderer(const Json::Value& value) {
	
	Component::FromJson(value);
	_type = type_component::MESHRENDERER;
	FromJson(value);
}

ComponentMeshRenderer::~ComponentMeshRenderer() {
	delete _mesh;
	_mesh = nullptr;

	delete _material;
	_material = nullptr;
}

void ComponentMeshRenderer::SetMesh(Mesh* component_mesh) {
	_mesh = component_mesh;
}

void ComponentMeshRenderer::SetMaterial(Material* component_material) {
	_material = component_material;
}

void ComponentMeshRenderer::DebugDraw(){
	
	assert(_owner != nullptr && "Component without and owner");
	
	if (_owner->IsSelected()) {
		AABB box;  _owner->GetAABB(box);
		OBB obb = box.Transform(_owner->GetGlobalMatrix());
		AABB res = obb.MinimalEnclosingAABB();
		dd::aabb(res.minPoint, res.maxPoint, dd::colors::Orange);
	}
}

void ComponentMeshRenderer::DrawFrame(int frameId, unsigned int texId, MatStandard_Flags_ flag) {

	MatStandard* matSt = (MatStandard*)_material;
	float2 image_size(24, 24);
	std::filesystem::path pwd;
	App->gui->_project->GetTexture(pwd);

	ImGui::BeginChildFrame(frameId, ImVec2(32, 32));
	
	float2 window_size(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	ImVec2 cursorPos((window_size.x - image_size.x) * 0.5f, (window_size.y - image_size.y) * 0.5f);
	ImGui::SetCursorPos(cursorPos);
	ImGui::Image((void*)texId, ImVec2(image_size.x, image_size.y));

	if (ImGui::BeginDragDropTarget() && IsImage(pwd.extension().string())) {

		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("explorer_drag_n_drop");
		if (payload) {
			//TODO: In a future --> The resource texture would be loaded, an then only
			// we need to change the resource texture
			// for now we load the texture via DevIL
			LOG(_INFO, "Loading texture: %s", pwd.filename().string());
			TextureInformation texture;
			App->texturer->LoadTexture(pwd.string().c_str(), texture);

			switch (flag)
			{
				case MatStandard_Flags_Albedo_Map:
					matSt->SetAlbedoMap(texture);
					break;
				case MatStandard_Flags_Specular_Map:
					matSt->SetSpecularMap(texture);
					break;
				case MatStandard_Flags_Normal_Map:
					matSt->SetNormalMap(texture);
					break;
			}
			

		}
		ImGui::EndDragDropTarget();
	}

	ImGui::EndChildFrame();
}

void ComponentMeshRenderer::OnEditor() {

	if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Text(_owner->GetName());
		ImGui::Separator();

		ImGui::BeginColumns("##meshrenderer",2,ImGuiColumnsFlags_NoBorder | ImGuiColumnsFlags_NoResize);
		ImGui::Text("Material");
		ImGui::NextColumn();
		ImGui::Text("%s", _material->GetName());
		ImGui::EndColumns();
	}

	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
		
		MatStandard* matSt = (MatStandard*)_material;
		
		static int current = 0;
		float3 color; matSt->SetColor(color);
		float smoothness = matSt->GetSmoothness();

		unsigned int texId;

		std::string id_com("##shader_"); id_com.append(_uuid);
		ImGui::PushID(id_com.c_str());
		ImGui::Combo("Shader", &current, shaderTypes, IM_ARRAYSIZE(shaderTypes));
		ImGui::PopID();
		ImGui::Separator();
		ImGui::Text("Main Maps");

		ImGui::BeginColumns("##material", 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		{
			float2 image_size(24, 24);
			//Albedo
			{
				
				DrawFrame(1,matSt->GetAlbedoMap(), MatStandard_Flags_Albedo_Map);
				ImGui::SameLine();
				ImGui::Text("Albedo");
			}
			//Specular
			{
				DrawFrame(2, matSt->GetSpecularMap(), MatStandard_Flags_Specular_Map);
				ImGui::SameLine();
				ImGui::Text("Specular");
			}

			ImGui::Text("Smoothness");

			//Normal
			{
				DrawFrame(3, matSt->GetNormalMap(), MatStandard_Flags_Normal_Map);
				ImGui::SameLine();
				ImGui::Text("Normal");
			}
		}
		ImGui::NextColumn();
		{

			std::string id_c("##color_"); id_c.append(_uuid);
			ImGui::PushID(id_c.c_str());
			if (ImGui::ColorEdit4("", &color[0],ImGuiColorEditFlags_NoInputs)) {
				matSt->SetColor(color);
			}
			ImGui::PopID();

			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::NewLine();

			std::string id_s("##smooth_"); id_s.append(_uuid);
			ImGui::PushID(id_s.c_str());
			if (ImGui::SliderFloat(id_s.c_str(), &smoothness, 0, 1)) {
				matSt->SetSmoothness(smoothness); 
			}
			ImGui::PopID();

		}
	}

}

void ComponentMeshRenderer::Render() {
	_mesh->Render(_material, _owner->GetGlobalMatrix());
}

bool ComponentMeshRenderer::ToJson(Json::Value& value, int pos)  {

	Component::ToJson(value, pos);
	value[pos][JSON_TAG_UUID] = _uuid;
	value[pos][JSON_TAG_MESH_PATH] = _mesh->GetFilePath();
	value[pos][JSON_TAG_MATERIAL_PATH] = _material->GetFilePath();

	switch (_material->GetType())
	{
	case SPECULAR_STANDARD:
	{
		MatStandard* aux = (MatStandard*)_material;
		aux->WriteJsonFile();
	}
	default:
		_material->WriteJsonFile();
		break;
	}


	return true;
}

bool ComponentMeshRenderer::FromJson(const Json::Value& value)  {

	if(!value.isNull())
	{
		std::ifstream ifs_mesh(value[JSON_TAG_MESH_PATH].asString());
		Json::CharReaderBuilder reader;
		Json::Value obj;
		std::string error;

		if (!Json::parseFromStream(reader, ifs_mesh, &obj, &error))
		{
			LOG(_ERROR, "Error parsing file: %s", error);
			return false;
		}

		_mesh = new Mesh();
		_mesh->FromJson(obj[0]);
		_mesh->SetFilePath(value[JSON_TAG_MESH_PATH].asCString());

		std::ifstream ifs_material(value[JSON_TAG_MATERIAL_PATH].asString());

		if (!Json::parseFromStream(reader, ifs_material, &obj, &error)) {
			LOG(_ERROR, "Error parsing file: %s", error);
			return false;
		}

		MatStandard* mat = new MatStandard();
		mat->FromJson(obj[0]);
		mat->SetFilePath(value[JSON_TAG_MATERIAL_PATH].asCString());
		_material = mat;

	}
	else 
	{
		return false;
	}
	return true;
}