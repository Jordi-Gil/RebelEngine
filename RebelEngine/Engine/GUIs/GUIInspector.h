#pragma once

#include "CoreModules/ModuleGUI.h"
#include "Math/float3.h"

class GameObject;
class ComponentCamera;
class ComponentMeshRenderer;
class ComponentTransform;

class GUIInspector : public ModuleGUI {

public:

	GUIInspector(const char* name);

	void Draw() override;
	void DrawGameObject(){}

	bool IsActive() const override { return _active; };

	const char* GetName() const override { return _name; }
	GameObject* GetFocusedGameObject() const { return _focused_go; }

	void ToggleActive();

	void SetFocusedGameObject(GameObject& focused);
	void ResetFocusedGameObject();
	
private:

	 GameObject* _focused_go = nullptr;

};

