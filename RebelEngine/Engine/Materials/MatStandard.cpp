#include "MatStandard.h"

#include <fstream>

#include <assimp/material.h>

#include "Main/Application.h"

#include "CoreModules/ModuleTexture.h"

MatStandard::MatStandard() {

	_type = material_type::SPECULAR_STANDARD;

}

MatStandard::MatStandard(const std::string name) {

	_name = name;
	_type = material_type::SPECULAR_STANDARD;

}

void MatStandard::LoadMaterialFromFBX(const aiMaterial* aiMat, const char* fbxPath) {

	//Diffuse texture - Albedo
	aiString file;
	unsigned int count;

	count = aiMat->GetTextureCount(aiTextureType_DIFFUSE);
	if (count > 0) {
		//By default only one diffuse map
		if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS) {
			if (App->texturer->LoadFromFBX(file.C_Str(), fbxPath, _maps[0])){ 
				_mask = _mask | MatStandard_Flags_Albedo_Map; 
			}
		}
	}

	count = aiMat->GetTextureCount(aiTextureType_SPECULAR);
	if (count > 0) {
		//By default only one diffuse map
		if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &file) == AI_SUCCESS) {
			if (App->texturer->LoadFromFBX(file.C_Str(), fbxPath, _maps[1])) { 
				_mask = _mask | MatStandard_Flags_Specular_Map; 
			}
		}
	}

	count = aiMat->GetTextureCount(aiTextureType_NORMALS);
	if (count > 0) {
		//By default only one diffuse map
		if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &file) == AI_SUCCESS) {
			if (App->texturer->LoadFromFBX(file.C_Str(), fbxPath, _maps[3])) { 
				_mask = _mask | MatStandard_Flags_Normal_Map; 
			}
		}
	}

	WriteJsonFile();

}

void MatStandard::GetTextureInformation(MatStandard_Flags_ flag, TextureInformation& texInfo) {

	if ((flag & MatStandard_Flags_Albedo_Map)) texInfo = _maps[0];
	else if ((flag & MatStandard_Flags_Specular_Map)) texInfo = _maps[1];
	else if ((flag & MatStandard_Flags_Normal_Map)) texInfo = _maps[2];

}

void MatStandard::SetColor(float3 color) {
	_color = color;
}

void MatStandard::SetSmoothness(float smoothness) {
	_smoothness = smoothness;
}

void MatStandard::SetAlbedoMap(TextureInformation albedo) {
	_maps[0] = albedo;
}

void MatStandard::SetSpecularMap(TextureInformation specular) {
	_maps[1] = specular;
}

void MatStandard::SetNormalMap(TextureInformation specular) {
	_maps[2] = specular;
}

bool MatStandard::WriteJsonFile() {

	Material::WriteJsonFile();

	_materialValue[0][JSON_TAG_MATERIAL_SMOOTHNESS] = _smoothness;

	Json::Value arr(Json::arrayValue); arr.append(_color.x); arr.append(_color.y); arr.append(_color.z);
	_materialValue[0][JSON_TAG_MATERIAL_COLOR] = arr;

	_materialValue[0][JSON_TAG_MATERIAL_PATH_ALBEDO] = _maps[0].path;
	_materialValue[0][JSON_TAG_MATERIAL_PATH_SPECULAR] = _maps[1].path;
	_materialValue[0][JSON_TAG_MATERIAL_PATH_NORMAL] = _maps[2].path;

	sprintf_s(_filePath, "%s%s%s", DEFAULT_MATERIAL_PATH, _name.c_str(), DEFAULT_MATERIAL_EXT);

	Json::StyledWriter wr;
	std::ofstream ofs(_filePath, std::ios_base::binary);
	std::string st = wr.write(_materialValue);
	ofs.write(st.c_str(), st.size());
	ofs.close();

	return true;
}

bool MatStandard::FromJson(const Json::Value& value) {

	if (!value.isNull())
	{

		Material::FromJson(value);
		_type = material_type::SPECULAR_STANDARD;

		_smoothness = value[JSON_TAG_MATERIAL_SMOOTHNESS].asFloat();
		std::string path = value[JSON_TAG_MATERIAL_PATH_ALBEDO].asCString();
		if (path != "") App->texturer->LoadTexture(path.c_str(), _maps[0]);
		path = value[JSON_TAG_MATERIAL_PATH_SPECULAR].asCString();
		if (path != "") App->texturer->LoadTexture(path.c_str(), _maps[1]);
		path = value[JSON_TAG_MATERIAL_PATH_NORMAL].asCString();
		if (path != "") App->texturer->LoadTexture(path.c_str(), _maps[2]);
		
	}
	else
	{
		return false;
	}
	return true;
}