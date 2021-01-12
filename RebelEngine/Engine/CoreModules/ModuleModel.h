#pragma once

#include "Module.h"
#include "Main/Mesh.h"
#include "Main/GameObject.h"

struct TextureInformation;

class ModuleModel : public Module {

friend class GUIInspector;

public:

	ModuleModel();

	bool Init() override;
	bool CleanUp() override;

	void LoadModel(const char* fileName);
	void LoadTexture(const char* fileName);

	//TODO: Move to a proper class
	//For this assigment this methods are implemented in this class
	void SetMinFilter(int i);
	void SetMagFilter(int i);
	void SetWrapS(int i);
	void SetWrapT(int i);

	std::vector<std::pair<unsigned int, TextureInformation>> textures;

private:

	void LoadTextures(aiMaterial** const materials, unsigned int mNumMaterials, const char* fbxPath);
	void LoadNodeHierarchy(aiNode* node, GameObject &father, const aiScene* scene, 
			const std::vector<std::string>& lights, const std::vector<std::string>& cameras, 
			const aiVector3D& aiScaling, const aiQuaternion& aiQuat, const aiVector3D& aiRot);

private:

	unsigned int numMaterials = 0;
};

