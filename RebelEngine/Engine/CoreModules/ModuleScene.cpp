#include "ModuleScene.h"
#include "Components/ComponentMeshRenderer.h"

ModuleScene::ModuleScene() {	
	root = std::make_unique<GameObject>();
}

bool ModuleScene::Init() {
	return true;
}

update_status ModuleScene::PreUpdate() {



	return UPDATE_CONTINUE;
}

void ModuleScene::DrawRecursive(GameObject &go) {

	if (go.GetNumChildren() != 0) {

		for (auto const& children : go.GetChildren()) {
			if (children->GetNumChildren() == 0) {
				if (children->HasComponent(type_component::MESHRENDERER)) {
					auto comp = static_cast<ComponentMeshRenderer&>(children->GetComponent(type_component::MESHRENDERER));
						comp.Draw();
				}
			}
			else {
				DrawRecursive(*children);
			}
		}

	}

}

void ModuleScene::Draw() {
	DrawRecursive(*root);
}
