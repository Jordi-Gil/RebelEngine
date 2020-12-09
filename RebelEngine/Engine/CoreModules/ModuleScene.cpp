#include "ModuleScene.h"

#include "Components/ComponentCamera.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"

#include "Main/Skybox.h"
#include "Main/GameObject.h"

ModuleScene::ModuleScene() {

	root = std::make_unique<GameObject>("Hierarchy");

	std::unique_ptr<ComponentTransform> transform = std::make_unique<ComponentTransform>();
	transform->SetOwner(root.get());
	root->AddComponent(std::move(transform));

	std::unique_ptr<GameObject> go = std::make_unique<GameObject>("Camera");
	transform = std::make_unique<ComponentTransform>();
	std::unique_ptr<ComponentCamera> cam = std::make_unique<ComponentCamera>();

	go->SetParent(root.get());
	transform->SetOwner(go.get());
	cam->SetOwner(go.get());

	go->AddComponent(std::move(transform));
	go->AddComponent(std::move(cam));

	root->AddChild(std::move(go));
}

ModuleScene::~ModuleScene() {

	delete goSelected;
	goSelected = nullptr;

	delete skybox;
	skybox = nullptr;

}

bool ModuleScene::Init() {

	skybox = new Skybox();

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
					ComponentMeshRenderer *comp = static_cast<ComponentMeshRenderer*>(children->GetComponent(type_component::MESHRENDERER));
						comp->Draw();
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
