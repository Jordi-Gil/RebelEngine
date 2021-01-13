#pragma once

#include "Main/Material.h"

#include "Math/float3.h"

#include "CoreModules/ModuleTexture.h"

struct aiMaterial;


enum MATSTANDARD_FLAGS {
	ALBEDO_MAP   = 1,
	SPECULAR_MAP = 1 << 2,
	NORMAL_MAP   = 1 << 3
};

class MatStandard : public Material {

public:

	MatStandard() : Material(material_type::SPECULAR_STANDARD) {};
	MatStandard(const std::string name);

	void LoadMaterialFromFBX(const aiMaterial* aiMat, const char* fbxPath);

	unsigned int GetAlbedoMap() const { return _albedo_map.gl_id; }
	unsigned int GetSpecularMap() const { return _specular_map.gl_id; }
	unsigned int GetNormalMap() const { return _normal_map.gl_id; }

	void GetTextureInformation(MATSTANDARD_FLAGS flag, TextureInformation& texInfo);

private:

	float3 color = float3(0.45, 0.45, 0.45);

	TextureInformation _albedo_map;
	TextureInformation _specular_map;
	TextureInformation _normal_map;

	int mask = 0;

	//unsigned int _height_map;
	//unsigned int _occulsion_map;
	//usnigned int _detail_mask;

};

