#pragma once

#include "CoreModules/ModuleGUI.h"
#include "Math/float3.h"

class GameObject;

class GUIInspector : public ModuleGUI {

public:

	GUIInspector(const char* name);

	void Draw() override;
	void DrawGameObject();
	bool IsActive() const override { return _active; };
	const char* GetName() const override { return _name; }

	void ToggleActive();
	void SetFocusedGameObject(GameObject& focused);
	void UnSetFocusedGameObject();
private:
	void UpdateTransform();
	void DrawDragFloat3(const char* name, float3& vector, float speed = 0.01f);
	
private:
	 GameObject* _focused_go = nullptr;
};

