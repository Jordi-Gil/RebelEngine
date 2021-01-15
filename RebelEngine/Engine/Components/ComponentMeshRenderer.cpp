#include "ComponentMeshRenderer.h"

#include <iostream>
#include <fstream>

#include "Math/float3x3.h"
#include "Math/float2.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "Main/Application.h"
#include "CoreModules/ModuleGUI.h"
#include "CoreModules/ModuleModel.h"
#include "CoreModules/ModuleTexture.h"
#include "ComponentTransform.h"
#include "Materials/MatStandard.h"
#include "GUIs/GUIProject.h"
#include "Utils/debugdraw.h"

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
	this->FromJson(value);
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

void ComponentMeshRenderer::OnEditor() {

	if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Text(_owner->GetName());
		ImGui::Separator();

		ImGui::BeginColumns("##meshrenderer",2,ImGuiColumnsFlags_NoBorder | ImGuiColumnsFlags_NoResize);
		ImGui::NextColumn();
		ImGui::Text("Material");
		ImGui::NextColumn();
		ImGui::Text("%s", _material->GetName());
		ImGui::EndColumns();
	}

	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
		
		MatStandard* matSt = (MatStandard*)_material;
		
		static int current = 0;
		static float3 color; matSt->SetColor(color);
		static float smoothness = matSt->GetSmoothness();

		unsigned int texId;

		std::filesystem::path pwd;
		App->gui->_project->GetTexture(pwd);

		ImGui::Combo("Shader", &current, shaderTypes, IM_ARRAYSIZE(shaderTypes));
		ImGui::Separator();
		ImGui::Text("Main Maps");

		ImGui::BeginColumns("##material", 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		{
			float2 image_size(24, 24);
			//Albedo
			{
				ImGui::BeginChildFrame(1, ImVec2(32, 32));

				texId = matSt->GetAlbedoMap();
				if (texId) {
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
							TextureInformation albedo;
							App->texturer->LoadTexture(pwd.string().c_str(), albedo);
							matSt->SetAlbedoMap(albedo);

						}
						ImGui::EndDragDropTarget();
					}

				}
				ImGui::EndChildFrame();

				ImGui::SameLine();
				ImGui::Text("Albedo");
			}
			//Specular
			{
				ImGui::BeginChildFrame(2, ImVec2(32, 32));
				texId = matSt->GetSpecularMap();
				if (texId != 0) {
					float2 window_size(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
					ImVec2 cursorPos((window_size.x - image_size.x) * 0.5f, (window_size.y - image_size.y) * 0.5f);
					ImGui::SetCursorPos(cursorPos);
					ImGui::Image((void*)texId, ImVec2(image_size.x, image_size.y));
				}
				ImGui::EndChildFrame();

				ImGui::SameLine();
				ImGui::Text("Specular");
			}

			ImGui::Text("Smoothness");

			//Normal
			{
				ImGui::BeginChildFrame(3, ImVec2(32, 32));
				texId = matSt->GetNormalMap();
				if (texId != 0) {
					float2 window_size(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
					ImVec2 cursorPos((window_size.x - image_size.x) * 0.5f, (window_size.y - image_size.y) * 0.5f);
					ImGui::SetCursorPos(cursorPos);
					ImGui::Image((void*)texId, ImVec2(image_size.x, image_size.y));
				}
				ImGui::EndChildFrame();

				ImGui::SameLine();
				ImGui::Text("Normal");
			}
		}
		ImGui::NextColumn();
		{
			if (ImGui::ColorEdit4("", &color[0],ImGuiColorEditFlags_NoInputs)) {
				matSt->SetColor(color);
			}

			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::NewLine();

			if (ImGui::SliderFloat("##smoothness", &smoothness, 0, 1)) { matSt->SetSmoothness(smoothness); }

		}
	}

}

void ComponentMeshRenderer::Render() {
	
	//TODO: check if active & change texture to Material
	_mesh->Draw(_material, _owner->GetGlobalMatrix());
}

bool ComponentMeshRenderer::ToJson(Json::Value& value, int pos) 
{
	Component::ToJson(value, pos);
	value[pos][JSON_TAG_UUID] = _uuid;
	value[pos][JSON_TAG_MESH_PATH] = _mesh->GetFilePath();

	return true;
}

bool ComponentMeshRenderer::FromJson(const Json::Value& value) 
{
	if(!value.isNull())
	{
		std::ifstream ifs(value[JSON_TAG_MESH_PATH].asString());
		Json::CharReaderBuilder reader;
		Json::Value obj;
		std::string error;

		if (!Json::parseFromStream(reader, ifs, &obj, &error)) 
		{
			LOG(_ERROR, "Error parsing file: %s", error);
			return false;
		}

		_mesh = new Mesh();
		_mesh->FromJson(obj[0]);
		_mesh->SetFilePath(value[JSON_TAG_MESH_PATH].asCString());
	}
	else 
	{
		return false;
	}
	return true;
}

