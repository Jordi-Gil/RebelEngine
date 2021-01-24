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

	void LoadModelFromFBX(const char* fileName);
	void LoadTexture(const char* fileName);

private:

	void LoadNodeHierarchy(const char* fileName, aiNode* node, GameObject &father, const aiScene* scene, 
			const std::vector<std::string>& lights, const std::vector<std::string>& cameras, 
			const aiVector3D& aiScaling, const aiQuaternion& aiQuat, const aiVector3D& aiRot);

private:

	unsigned int numMaterials = 0;
};

