#pragma once

#include <map>

#include "Module.h"

#include "Main/Mesh.h"

struct TextureInformation;

class ModuleModel : public Module {

friend class GUIInspector;

public:

	ModuleModel();

	bool Init();

	void LoadModel(const char* file_name);

	bool CleanUp();

	void Draw();

private:

	void LoadMeshes(aiMesh** const mMeshes, unsigned int mNumMeshes);
	void LoadTextures(aiMaterial** const materials, unsigned int mNumMaterials, const char* fbxPath);

public:

	float max[3] = { FLT_MIN, FLT_MIN , FLT_MIN }, min[3] = { FLT_MAX , FLT_MAX , FLT_MAX };

private:

	std::vector <Mesh> meshes;
	unsigned int numMeshes;
	unsigned int numMaterials;
	std::vector<std::pair<unsigned int, TextureInformation>> textures;
};

