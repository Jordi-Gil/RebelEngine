#include "ModuleModel.h"

#include <GL/GL.h>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>

#include "Main/Application.h"

#include "ModuleEditorCamera.h"
#include "ModuleTexture.h"
#include "ModuleScene.h"

#include "Utils/Globals.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"

#include "Materials/MatStandard.h"

void LogAssimp(const char* msg, char* userData) {
	if(msg) LOG(_INFO, "Assimp Message: %s", msg);
}

ModuleModel::ModuleModel() {}

bool ModuleModel::Init() {

	// Register it
	struct aiLogStream stream;
	stream.callback = LogAssimp;
	aiAttachLogStream(&stream);

	//LoadModelFromFBX("Assets/Models/WithDDS/BakerHouse/BakerHouse.fbx");
	LoadModelFromFBX("Assets/Models/Unity/Robot/Robot.FBX");
	//LoadModelFromFBX("Assets/Models/WithoutDDS/Street_Environment/Street_environment_V01.FBX");
	
	return true;
}

void LoadMesh(Mesh& mesh, const char* name, aiMesh* aimesh) {

	mesh.SetName(name);
	mesh.LoadVBO(aimesh);
	mesh.LoadEBO(aimesh);
	mesh.CreateVAO();
	mesh.WriteJsonFile();

}

void LoadMaterialSpec(MatStandard& material, const char* name, aiMaterial* aiMat, const char *fileName) {

	std::string _name = name; _name.append("_"); _name.append(material.GetUUID());
	material.SetName(_name);
	material.LoadMaterialFromFBX(aiMat, fileName);

}

void ModuleModel::LoadModelFromFBX(const char* fileName) {
	
	const aiScene* scene = aiImportFile(fileName, 
		aiProcessPreset_TargetRealtime_MaxQuality
	);

	if (scene) {

		char fn[_MAX_FNAME]; _splitpath_s(fileName, NULL, 0, NULL, 0, fn, _MAX_FNAME, NULL, 0);

		std::vector<std::string> lights;
		std::vector<std::string> cameras;

		//Retrieve light and cameras information
		if (scene->HasLights()) {
			for (uint i = 0; i < scene->mNumLights; ++i) {
				lights.push_back(scene->mLights[i]->mName.C_Str());
			}
		}

		if (scene->HasCameras()) {
			for (uint i = 0; i < scene->mNumCameras; ++i) {
				cameras.push_back(scene->mCameras[i]->mName.C_Str());
			}
		}

		aiNode* father = scene->mRootNode;

		//Decompose Root Matrix
		aiVector3D aiScaling; aiQuaternion aiQuat; aiVector3D aiPos;
		aiDecomposeMatrix(&aiMatrix4x4(), &aiScaling, &aiQuat, &aiPos);

		std::unique_ptr<GameObject> go = App->scene->_poolGameObjects.get(); go->SetName(fn);
		std::unique_ptr<ComponentTransform> transform = std::make_unique<ComponentTransform>();

		go->SetParent(App->scene->_root.get());

		transform->SetOwner(go.get());
		go->AddComponent(std::move(transform));

		LoadNodeHierarchy(fileName, father, *go, scene, lights, cameras, aiPos, aiQuat, aiScaling);
		App->scene->_root->AddChild(std::move(go));

		LOG(_INFO, "3D Model %s loaded", fn);

		aiReleaseImport(scene);

	}
	else {
		LOG(_ERROR, "Error loading mesh %s: %s", fileName, aiGetErrorString());
	}

}

void ModuleModel::LoadNodeHierarchy(const char* fileName, aiNode *node, GameObject &father, const aiScene* scene, 
	const std::vector<std::string> &lights, const std::vector<std::string>& cameras, 
	const aiVector3D& aiTrans, const aiQuaternion& aiQuat, const aiVector3D& aiScaling) {

	// assignar nombre edl mesh al name del game object
	int unsigned num_children = node->mNumChildren;
	
	for (unsigned int i = 0; i < num_children; ++i) {//node iteration

		std::string str = node->mChildren[i]->mName.C_Str();

		aiVector3D aiScaling_aux; aiQuaternion aiQuat_aux; aiVector3D aiTrans_aux;

		if (str.find("$AssimpFbx$") != std::string::npos) {

			aiDecomposeMatrix(&node->mChildren[i]->mTransformation, &aiScaling_aux, &aiQuat_aux, &aiTrans_aux);
			aiScaling_aux.SymMul(aiScaling);
			aiTrans_aux = aiTrans_aux + aiTrans;
			aiQuaternion aiQuat_aux2 = aiQuat_aux * aiQuat;

			LoadNodeHierarchy(fileName, node->mChildren[i], father, scene, lights, cameras, 
				aiTrans_aux, aiQuat_aux2, aiScaling_aux);
		}
		else {

			aiDecomposeMatrix(&node->mChildren[i]->mTransformation, &aiScaling_aux, &aiQuat_aux, &aiTrans_aux);
			aiScaling_aux.SymMul(aiScaling);
			aiTrans_aux = aiTrans_aux + aiTrans;
			aiQuat_aux = aiQuat_aux * aiQuat;

			std::unique_ptr<GameObject> go = App->scene->_poolGameObjects.get();
			go->SetName(node->mChildren[i]->mName.C_Str());
			std::unique_ptr <ComponentTransform> transform = 
				std::make_unique <ComponentTransform>(aiMatrix4x4(aiScaling_aux, aiQuat_aux, aiTrans_aux));

			go->SetParent(&father);

			transform->SetOwner(go.get());
			go->AddComponent(std::move(transform));

			//To avoid create an empty GameObject between parent and the game object wiht mesh itself.
			if (node->mChildren[i]->mNumMeshes == 1) {

				std::unique_ptr<ComponentMeshRenderer> renderer_mesh = std::make_unique<ComponentMeshRenderer>();

				Mesh* mesh = new Mesh();
				LoadMesh(*mesh, node->mChildren[i]->mName.C_Str(), scene->mMeshes[node->mChildren[i]->mMeshes[0]]);
				
				int mIdx = scene->mMeshes[node->mChildren[i]->mMeshes[0]]->mMaterialIndex;
				//TODO: Load materials separately and search the material by name in the Resources manager
				//		and assign it to the meshrenderer. Name material equal than mesh
				//		scene->mMaterials[mIdx]->GetName().C_Str()
				MatStandard* material = new MatStandard();
				LoadMaterialSpec(*material, node->mChildren[i]->mName.C_Str(), scene->mMaterials[mIdx], fileName);

				renderer_mesh->SetOwner(go.get());
				renderer_mesh->SetMesh(mesh);
				renderer_mesh->SetMaterial(material);
				go->AddComponent(std::move(renderer_mesh), GO_MASK_MESH);

			}
			else {
				for (unsigned int x = 0; x < node->mChildren[i]->mNumMeshes; ++x) {//mesh iteration

					std::unique_ptr<GameObject> go_mesh = App->scene->_poolGameObjects.get();
					go_mesh->SetName(scene->mMeshes[node->mChildren[i]->mMeshes[x]]->mName.C_Str());
					go_mesh->SetParent(go.get());

					std::unique_ptr<ComponentMeshRenderer> renderer_mesh = std::make_unique<ComponentMeshRenderer>();
					std::unique_ptr <ComponentTransform> transform_mesh =  std::make_unique <ComponentTransform>(aiMatrix4x4(aiScaling_aux, aiQuat_aux, aiTrans_aux));

					transform_mesh->SetOwner(go_mesh.get());
					renderer_mesh->SetOwner(go_mesh.get());

					Mesh* mesh = new Mesh();
					LoadMesh(*mesh, node->mChildren[i]->mName.C_Str(), scene->mMeshes[node->mChildren[i]->mMeshes[x]]);

					int mIdx = scene->mMeshes[node->mChildren[i]->mMeshes[x]]->mMaterialIndex;
					//TODO: Load materials separately and search the material by name in the Resources manager
					//		and assign it to the meshrenderer. Name material equal than mesh
					//		scene->mMaterials[mIdx]->GetName().C_Str()
					MatStandard* material = new MatStandard();
					LoadMaterialSpec(*material, node->mChildren[i]->mName.C_Str(), scene->mMaterials[mIdx], fileName);

					renderer_mesh->SetMesh(mesh);
					renderer_mesh->SetMaterial(material);

					go_mesh->AddComponent(std::move(transform_mesh));
					go_mesh->AddComponent(std::move(renderer_mesh), GO_MASK_MESH);

					go->AddChild(std::move(go_mesh));
				}
			}

			go->SetParent(&father);
			LoadNodeHierarchy(fileName, node->mChildren[i], *go, scene, lights, cameras, aiVector3D(0,0,0), aiQuaternion(), aiVector3D(1,1,1));
			father.AddChild(std::move(go));

		}
	}
}

bool ModuleModel::CleanUp() {

	aiDetachAllLogStreams();

	return true;
}

void ModuleModel::SetMinFilter(int i) {
	//for (unsigned int j = 0; j < textures.size(); j++) {
	//	App->texturer->SetMinFilter(i, textures[j].first);
	//}
}

void ModuleModel::SetMagFilter(int i) {
	//for (unsigned int j = 0; j < textures.size(); j++) {
	//	App->texturer->SetMagFilter(i, textures[j].first);
	//}
}

void ModuleModel::SetWrapS(int i) {
	//for (unsigned int j = 0; j < textures.size(); j++) {
	//	App->texturer->SetWrapS(i, textures[j].first);
	//}
}

void ModuleModel::SetWrapT(int i) {
	//for (unsigned int j = 0; j < textures.size(); j++) {
	//	App->texturer->SetWrapT(i, textures[j].first);
	//}
}
