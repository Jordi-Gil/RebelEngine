#pragma once

#include <map>

#include "Module.h"

#include "Main/Mesh.h"

class ModuleModel : public Module
{
public:

	ModuleModel();

	bool Init();

	void LoadModel(const char* file_name);

	bool CleanUp();

	void Draw();

private:

	void LoadMeshes(aiMesh** const mMeshes, unsigned int mNumMeshes);
	void LoadTextures(aiMaterial** const materials, unsigned int mNumMaterials);
	
private:

	std::vector <Mesh> meshes;
	unsigned int numMeshes;
	std::vector<unsigned int> textures;
	unsigned int numMaterials;
};

