#pragma once

#include "Materials/Material.h"

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

	MatStandard();
	MatStandard(const std::string name);

	void LoadMaterialFromFBX(const aiMaterial* aiMat, const char* fbxPath);

	unsigned int GetAlbedoMap() const { return _albedo_map.gl_id; }
	unsigned int GetSpecularMap() const { return _specular_map.gl_id; }
	unsigned int GetNormalMap() const { return _normal_map.gl_id; }

	float GetSmoothness() const { return _smoothness; }

	void GetTextureInformation(MATSTANDARD_FLAGS flag, TextureInformation& texInfo);

	void GetColor(float3& color) const { color = _color; }

	void SetColor(float3 color);
	void SetSmoothness(float smoothness);
	void SetAlbedoMap(TextureInformation albedo);

	bool FromJson(const Json::Value& value);
	bool WriteJsonFile();

private:

	int _mask = 0;
	float _smoothness = 1;

	float3 _color = float3(0.45, 0.45, 0.45);

	TextureInformation _albedo_map;
	TextureInformation _specular_map;
	TextureInformation _normal_map;

	//unsigned int _height_map;
	//unsigned int _occulsion_map;
	//usnigned int _detail_mask;

};

