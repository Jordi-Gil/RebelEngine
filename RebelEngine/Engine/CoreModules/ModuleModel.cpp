#include "ModuleModel.h"
#include "ModuleTexture.h"
#include "ModuleEditorCamera.h"

#include "Main/Application.h"

#include "Utils/Globals.h"

#include <GL/GL.h>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>


void myCallback(const char* msg, char* userData) {
	if(msg) LOG(_INFO, "Assimp Message: %s", msg);
}

ModuleModel::ModuleModel() {}

bool ModuleModel::Init() {

	// Register it
	struct aiLogStream stream;
	stream.callback = myCallback;
	aiAttachLogStream(&stream);

	LoadModel("Assets/Models/BakerHouse.fbx");
	return true;
}

void ModuleModel::LoadMeshes(aiMesh** const mMeshes, unsigned int mNumMeshes){
	
	LOG(_INFO, "Meshes: %d", mNumMeshes);

	numMeshes = mNumMeshes;

	meshes.reserve(numMeshes);

	for (unsigned int i = 0; i < mNumMeshes; i++) {
		meshes.push_back(Mesh());
		meshes[i].LoadVBO(mMeshes[i], max, min);
		meshes[i].LoadEBO(mMeshes[i]);
		meshes[i].CreateVAO();
	}

	App->editorCamera->SetPosition( (max[0] + min[0])/2, (max[1] + min[1])/2, (max[2] + min[2])/2 + 10 );

}

void ModuleModel::LoadTextures(aiMaterial** const materials, unsigned int mNumMaterials, const char * fbxPath) {

	numMaterials = mNumMaterials;

	textures.reserve(numMaterials);

	aiString file;

	for (unsigned int i = 0; i < numMaterials; ++i) {
		if (materials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS) {
			char dir[_MAX_DIR];
			errno_t error = _splitpath_s(fbxPath, NULL, 0, dir, _MAX_DIR, NULL, 0, NULL, 0);
			if (error != 0) {
				LOG(_ERROR, "Couldn't split the given path. Error: ", strerror(error));
			}
			textures.push_back(App->texturer->loadTexture(file.data, dir));
		}
	}
}

void ModuleModel::LoadModel(const char* file_name) {
	
	//For the assigment one. If new mesh is loaded, clear all arrays (vectors)
	if (!meshes.empty()) {
		CleanUp();
		max[0] = max[1] = max[2] = FLT_MIN;
		min[0] = min[1] = min[2] = FLT_MAX;
	}

	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene) {

		LoadMeshes(scene->mMeshes, scene->mNumMeshes);
		LoadTextures(scene->mMaterials, scene->mNumMaterials, file_name);
	}
	else {
		LOG(_ERROR, "Error loading %s: %s", file_name, aiGetErrorString());
	}

}

void ModuleModel::Draw() {

	for (unsigned int i = 0; i < numMeshes ; ++i) {
		meshes[i].Draw(textures);
	}

}

bool ModuleModel::CleanUp() {

	for (auto mesh : meshes) mesh.Clean();
	meshes.clear();

	for (auto texture : textures) {
		glDeleteTextures(1, &texture);
	}

	textures.clear();
	return true;
}