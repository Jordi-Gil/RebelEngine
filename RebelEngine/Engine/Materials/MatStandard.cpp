#include "MatStandard.h"

#include <assimp/material.h>

#include "Main/Application.h"

#include "CoreModules/ModuleTexture.h"

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
			if (App->texturer->LoadFromFBX(file.C_Str(), fbxPath, _albedo_map)){ mask = mask | ALBEDO_MAP; }
		}
	}

	count = aiMat->GetTextureCount(aiTextureType_SPECULAR);
	if (count > 0) {
		//By default only one diffuse map
		if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &file) == AI_SUCCESS) {
			if (App->texturer->LoadFromFBX(file.C_Str(), fbxPath, _specular_map)) { mask = mask | SPECULAR_MAP; }
		}
	}

	count = aiMat->GetTextureCount(aiTextureType_NORMALS);
	if (count > 0) {
		//By default only one diffuse map
		if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &file) == AI_SUCCESS) {
			if (App->texturer->LoadFromFBX(file.C_Str(), fbxPath, _normal_map)) { mask = mask | NORMAL_MAP; }
		}
	}


}

void MatStandard::GetTextureInformation(MATSTANDARD_FLAGS flag, TextureInformation& texInfo) {

	if ((flag & ALBEDO_MAP)) texInfo = _albedo_map;
	else if ((flag & SPECULAR_MAP)) texInfo = _specular_map;
	else if ((flag & NORMAL_MAP)) texInfo = _normal_map;

}
