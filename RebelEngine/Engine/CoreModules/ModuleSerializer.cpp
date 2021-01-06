#include "ModuleSerializer.h"

#include "Main/GameObject.h"
#include "Utils/Globals.h"
#include "Utils/RUUID.h"

#include <iostream>
#include <fstream>

//bool ModuleSerializer::Init() {
//    
//    std::ifstream ifs("./Assets/Scenes/example.json");
//    Json::CharReaderBuilder reader;
//    Json::Value obj;
//    std::string error;
//
//    if (!Json::parseFromStream(reader, ifs, &obj, &error)) {
//        LOG(_ERROR, "Error parsing file: %s", error);
//        return false;
//    }
//
//    std::string pruebas = obj["book"].asString();
//    LOG(_INFO, "Book: %s", obj["book"].asString());
//
//	return true;
//}
//

//bool ModuleSerializer::SceneToJson(const std::unique_ptr<GameObject>& go, Json::Value& list)
//{
//
//    Json::Value childrenList;
//    Json::Value componentList;
//    std::vector<std::unique_ptr<GameObject>>& children = go.get()->GetChildren();
//    std::vector<std::unique_ptr<Component>>& components = go.get()->GetComponents();
//
//    jsonRoot[0]["Name"] = DEFAULT_SCENE_NAME;
//
//    for (const auto &child : children)
//    {
//        GameObjectToJsonRec(child, childrenList);
//    }
//    jsonRoot[0]["GameObjects"] = childrenList;
//
//    int comp_cont = 0;
//    for (const auto& comp : components) 
//    {
//
//        ++comp_cont;
//    }
//    jsonRoot[0]["Components"] = componentList;
//
//    return true;
//
//}
//
//bool ModuleSerializer::SaveScene(const char* filePath, const ModuleScene &scene)
//{
//
//    std::string s = RUUID::generate_uuid_v4();
//    jsonRoot[0]["SceneName"] = DEFAULT_SCENE_NAME;
//    jsonRoot[0]["UUID"] = s;
//    Json::Value gameObjects;
//    
//    GameObjectToJsonRec(scene._root, gameObjects);
//
//    jsonRoot[0]["GameObjects"] = gameObjects;
//    
//    Json::StyledWriter wr;
//    std::ofstream ofs("./Assets/Scenes/example2.json");
//    std::string st = wr.write(jsonRoot);
//    ofs.write(st.c_str(), st.size());
//
//    return true;
//
//}