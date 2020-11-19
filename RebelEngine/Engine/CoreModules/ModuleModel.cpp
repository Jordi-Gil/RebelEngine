#include "ModuleModel.h"
#include "ModuleTexture.h"

#include "Main/Application.h"

#include "Utils/Globals.h"
#include "Utils/Console.h"

#include <GL/GL.h>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>


ModuleModel::ModuleModel() {}

bool ModuleModel::Init() {

	LoadModel("BakerHouse.fbx");

	return true;
}

void ModuleModel::LoadMeshes(aiMesh** const mMeshes, unsigned int mNumMeshes){
	
	LOG(_INFO, "Meshes: %d", mNumMeshes);

	numMeshes = mNumMeshes;

	meshes.reserve(numMeshes);

	for (unsigned int i = 0; i < mNumMeshes; i++) {
		meshes.push_back(Mesh());
		meshes[i].LoadVBO(mMeshes[i]);
		meshes[i].LoadEBO(mMeshes[i]);
		meshes[i].CreateVAO();
	}
}

void ModuleModel::LoadTextures(aiMaterial** const materials, unsigned int mNumMaterials) {

	numMaterials = mNumMaterials;

	textures.reserve(numMaterials);

	aiString file;

	for (unsigned int i = 0; i < numMaterials; ++i) {
		if (materials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS) {
			textures.push_back(App->texturer->loadTexture(file.data));
		}
	}
}

void ModuleModel::LoadModel(const char* file_name) {
	
	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene) {

		LoadMeshes(scene->mMeshes, scene->mNumMeshes);
		LoadTextures(scene->mMaterials, scene->mNumMaterials);
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