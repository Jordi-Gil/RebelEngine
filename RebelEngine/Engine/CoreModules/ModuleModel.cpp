#include "ModuleModel.h"
#include "ModuleTexture.h"
#include "ModuleEditorCamera.h"
#include "ModuleScene.h"

#include "Main/Application.h"
#include "Utils/Globals.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"

#include <GL/GL.h>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>

void LogAssimp(const char* msg, char* userData) {
	if(msg) LOG(_INFO, "Assimp Message: %s", msg);
}

ModuleModel::ModuleModel() {}

bool ModuleModel::Init() {

	// Register it
	struct aiLogStream stream;
	stream.callback = LogAssimp;
	aiAttachLogStream(&stream);

	LoadModel("Assets/Models/WithDDS/BakerHouse/BakerHouse.fbx");
	//LoadModel("Assets/Models/WithoutDDS/Street_Environment/Street_environment_V01.FBX");
	return true;
}

void ModuleModel::LoadTexture(const char* fileName) {
	
	if (!textures.empty()) {
		textures.clear();
		textures.shrink_to_fit();
	}

	TextureInformation info;
	unsigned int textureId = App->texturer->loadTexture(fileName, info);
	textures.push_back(std::make_pair(textureId, info));

}

void ModuleModel::LoadTextures(aiMaterial** const materials, unsigned int mNumMaterials, const char * fbxPath) {

	numMaterials = mNumMaterials;

	textures.reserve(numMaterials);

	aiString file;

	for (unsigned int i = 0; i < numMaterials; ++i) {
		if (materials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS) {

			TextureInformation info;

			LOG(_INFO, "Loading texture from %s", file.data);
			unsigned int textureId = App->texturer->loadTexture(file.data, info);

			char path[_MAX_PATH]; char dir[_MAX_DIR]; char fileName[_MAX_FNAME], extension[_MAX_EXT];
			errno_t error = _splitpath_s(file.data, NULL, 0, NULL, 0, fileName, _MAX_FNAME, extension, _MAX_EXT);
			if (error != 0) { 
				char errmsg[256];
				strerror_s(errmsg, 256, error);
				LOG(_ERROR, "Couldn't split the given path %s. Error: %s", file.data, errmsg);
				
				return; 
			}

			if (textureId == 0) {
				//If textureID == 0 , load fails from path given by FBX.
				//Load from same directory than mesh file.
				error = _splitpath_s(fbxPath, NULL, 0, dir, _MAX_DIR, NULL, 0, NULL, 0);
				if (error != 0) {
					char errmsg[256];
					strerror_s(errmsg, 256, error);
					LOG(_ERROR, "Couldn't split the given path %s. Error: %s", fbxPath, errmsg);
					return; 
				}

				error = sprintf_s(path, _MAX_PATH, "%s%s%s", dir, fileName, extension);
				if (error == -1) { char errmsg[256]; strerror_s(errmsg, 256, error); LOG(_ERROR, "Error constructing path. Error: %s", errmsg); return; }

				LOG(_INFO, "Loading texture from %s", path);
				textureId = App->texturer->loadTexture(path, info);
			}
			else LOG(_INFO, "Texture loaded from path: %s", file.data);

			if (textureId == 0) {
				//If textureID == 0 , load fails from same directory than FBX.
				//Load from textures directory.
				error = sprintf_s(path, _MAX_PATH, "Assets/Textures/%s%s", fileName, extension);
				if (error == -1) { char errmsg[256]; strerror_s(errmsg, 256, error); LOG(_ERROR, "Error constructing path. Error: %s", errmsg); return; }

				LOG(_INFO, "Loading texture from %s", path);
				textureId = App->texturer->loadTexture(path, info);
				if(textureId != 0) LOG(_INFO, "Texture loaded from path: %s", path);
			}
			else LOG(_INFO, "Texture loaded from path: %s", path);

			if (textureId != 0) {
				info.name += fileName; info.name += extension;
				textures.push_back(std::make_pair(textureId, info));
			}
			else {
				textureId = App->texturer->loadTexture("Assets/Textures/pink.dds", info);
				if (textureId != 0) {
					info.name += "pink.dds";
					textures.push_back(std::make_pair(textureId, info));
				}
				LOG(_ERROR, "Impossible to load texture.");
			}
		}
		else {
			TextureInformation info;
			unsigned int textureId = App->texturer->loadTexture("Assets/Textures/pink.dds", info);
			if (textureId != 0) {
				info.name += "pink.dds";
				textures.push_back(std::make_pair(textureId, info));
			}
		}
	}
}

void ModuleModel::LoadModel(const char* fileName) {
	
	const aiScene* scene = aiImportFile(fileName, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene) {

		char fn[_MAX_FNAME]; _splitpath_s(fileName, NULL, 0, NULL, 0, fn, _MAX_FNAME, NULL, 0);

		aiNode* father = scene->mRootNode;

		std::unique_ptr<GameObject> go = std::make_unique<GameObject>(_strdup(fn));
		std::unique_ptr<ComponentTransform> transform = std::make_unique<ComponentTransform>();

		go->SetParent(App->scene->root.get());

		transform->SetOwner(go.get());
		go->AddComponent(std::move(transform));

		LoadNodeHierarchy(father, *go, scene);
		App->scene->root->AddChild(std::move(go));

		LoadTextures(scene->mMaterials, scene->mNumMaterials, fileName);
		LOG(_INFO, "3D Model %s loaded", _strdup(fn));
	}
	else {
		LOG(_ERROR, "Error loading mesh %s: %s", fileName, aiGetErrorString());
	}

}

void ModuleModel::LoadNodeHierarchy(aiNode *node, GameObject &father, const aiScene* scene) {

	// assignar nombre edl mesh al name del game object
	int unsigned num_children = node->mNumChildren;
	
	for (unsigned int i = 0; i < num_children; ++i) {//node iteration

		std::unique_ptr<GameObject> go = std::make_unique<GameObject>(node->mChildren[i]->mName.C_Str());
		std::unique_ptr <ComponentTransform> transform = std::make_unique <ComponentTransform>(node->mChildren[i]->mTransformation);

		go->SetParent(&father);

		transform->SetOwner(go.get());
		go->AddComponent(std::move(transform));

		if (node->mChildren[i]->mNumMeshes == 1) {
			
			std::unique_ptr<ComponentMeshRenderer> renderer_mesh = std::make_unique<ComponentMeshRenderer>();

			Mesh* mesh = new Mesh();
			mesh->LoadVBO(scene->mMeshes[node->mChildren[i]->mMeshes[0]]);
			mesh->LoadEBO(scene->mMeshes[node->mChildren[i]->mMeshes[0]]);
			mesh->CreateVAO();

			renderer_mesh->SetOwner(go.get());
			renderer_mesh->SetMesh(mesh);
			go->AddComponent(std::move(renderer_mesh));
		}
		else {
			for (unsigned int x = 0; x < node->mChildren[i]->mNumMeshes; ++x) {//mesh iteration

				std::unique_ptr<GameObject> go_mesh = std::make_unique<GameObject>(scene->mMeshes[node->mChildren[i]->mMeshes[x]]->mName.C_Str());
				std::unique_ptr<ComponentMeshRenderer> renderer_mesh = std::make_unique<ComponentMeshRenderer>();
				std::unique_ptr <ComponentTransform> transform_mesh = std::make_unique <ComponentTransform>(node->mChildren[i]->mTransformation);

				transform_mesh->SetOwner(go_mesh.get());
				renderer_mesh->SetOwner(go_mesh.get());

				Mesh* mesh = new Mesh();
				mesh->LoadVBO(scene->mMeshes[node->mChildren[i]->mMeshes[x]]);
				mesh->LoadEBO(scene->mMeshes[node->mChildren[i]->mMeshes[x]]);
				mesh->CreateVAO();

				renderer_mesh->SetMesh(mesh);

				go_mesh->AddComponent(std::move(transform_mesh));
				go_mesh->AddComponent(std::move(renderer_mesh));
				go_mesh->SetParent(go.get());

				go->AddChild(std::move(go_mesh));
			}
		}
		go->SetParent(&father);
		LoadNodeHierarchy(node->mChildren[i], *go, scene);
		father.AddChild(std::move(go));
	}
	
}

bool ModuleModel::CleanUp() {

	for (unsigned i = 0; i < textures.size(); i++) glDeleteTextures(1, &textures[i].first);
	textures.clear();
	textures.shrink_to_fit();

	return true;
}

void ModuleModel::SetMinFilter(int i) {
	for (unsigned int j = 0; j < textures.size(); j++) {
		App->texturer->SetMinFilter(i, textures[j].first);
	}
}

void ModuleModel::SetMagFilter(int i) {
	for (unsigned int j = 0; j < textures.size(); j++) {
		App->texturer->SetMagFilter(i, textures[j].first);
	}
}

void ModuleModel::SetWrapS(int i) {
	for (unsigned int j = 0; j < textures.size(); j++) {
		App->texturer->SetWrapS(i, textures[j].first);
	}
}

void ModuleModel::SetWrapT(int i) {
	for (unsigned int j = 0; j < textures.size(); j++) {
		App->texturer->SetWrapT(i, textures[j].first);
	}
}
