#pragma once
#include "Module.h"
#include "ModuleScene.h"
#include "json/json.h"

class ModuleSerializer : public Module
{
public:

	//bool Init() override;

	//bool SaveScene(const char* filePath,const ModuleScene& scene);
	//bool LoadScene(const char* filePath);

private:
	Json::Value jsonRoot;
private:
	//bool GameObjectToJsonRec(const std::unique_ptr<GameObject>& go, Json::Value& list);
};

