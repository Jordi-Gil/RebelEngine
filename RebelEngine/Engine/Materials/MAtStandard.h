#pragma once

#include "Materials/Material.h"

#include "Math/float3.h"

#include "CoreModules/ModuleTexture.h"

struct aiMaterial;


enum MatStandard_Flags_ {
	MatStandard_Flags_Albedo_Map   = 1,
	MatStandard_Flags_Specular_Map = 1 << 2,
	MatStandard_Flags_Normal_Map   = 1 << 3
};

class MatStandard : public Material {

public:

	MatStandard();
	MatStandard(const std::string name);

	void LoadMaterialFromFBX(const aiMaterial* aiMat, const char* fbxPath);

	unsigned int GetAlbedoMap() const { return _maps[0].gl_id; }
	unsigned int GetSpecularMap() const { return _maps[1].gl_id; }
	unsigned int GetNormalMap() const { return _maps[2].gl_id; }

	float GetSmoothness() const { return _smoothness; }

	void GetTextureInformation(MatStandard_Flags_ flag, TextureInformation& texInfo);

	void GetColor(float3& color) const { color = _color; }

	void SetColor(float3 color);
	void SetSmoothness(float smoothness);
	void SetAlbedoMap(TextureInformation albedo);
	void SetSpecularMap(TextureInformation specular);
	void SetNormalMap(TextureInformation normal);

	bool FromJson(const Json::Value& value);
	bool WriteJsonFile();

private:

	friend class Mesh;

	int _mask = 0;
	float _smoothness = 1;

	float3 _color = float3(0.45, 0.45, 0.45);

	TextureInformation _maps[3];

	//unsigned int _height_map;
	//unsigned int _occulsion_map;
	//usnigned int _detail_mask;

};

