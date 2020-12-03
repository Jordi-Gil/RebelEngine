#pragma once

#include "Module.h"
#include "Main/Mesh.h"
#include "Main/GameObject.h"

#include "Math/float3.h"

struct TextureInformation;

class ModuleModel : public Module {

friend class GUIInspector;

public:

	ModuleModel();

	bool Init();
	bool CleanUp();

	void LoadModel(const char* fileName);
	void LoadTexture(const char* fileName);

	void Draw();

	//TODO: Move to a prope class, e.g. scene manager
	vec GetSizeScene() {
		return vec(max[0] - min[0], max[1] - min[1], max[2] - min[2]);
	}
	vec GetCenterScene() {
		return vec(
			(max[0] + min[0]) / 2,
			(max[1] + min[1]) / 2,
			(max[2] + min[2]) / 2
		);
	}

	//TODO: Move to a proper class
	//For this assigment this methods are implemented in this class
	void SetMinFilter(int i);
	void SetMagFilter(int i);
	void SetWrapS(int i);
	void SetWrapT(int i);

private:

	void LoadMeshes(aiMesh** const mMeshes, unsigned int mNumMeshes);
	void LoadMeshes(aiMesh** const mMeshes, unsigned int mNumMeshes, GameObject& father);
	void LoadTextures(aiMaterial** const materials, unsigned int mNumMaterials, const char* fbxPath);
	void LoadNodeHierarchy(aiNode* node, std::unique_ptr<GameObject> father,const aiScene* scene);
private:

	float max[3] = { FLT_MIN, FLT_MIN , FLT_MIN }, min[3] = { FLT_MAX , FLT_MAX , FLT_MAX };

	std::vector <Mesh> meshes;
	unsigned int numMeshes;
	unsigned int numMaterials;
	std::vector<std::pair<unsigned int, TextureInformation>> textures;
};

