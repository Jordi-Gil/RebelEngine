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

void ModuleScene::DrawRecursive(GameObject &go) {

	for (auto const& children : go.GetChildren()) {
		DrawRecursive(*children);
	}

	if (go.HasComponent(type_component::MESHRENDERER)) {
		ComponentMeshRenderer* comp = static_cast<ComponentMeshRenderer*>(go.GetComponent(type_component::MESHRENDERER));
		comp->Draw();
	}

}

void ModuleScene::Draw() {
	DrawRecursive(*root);
}
