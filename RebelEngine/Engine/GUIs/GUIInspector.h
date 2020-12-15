#pragma once

#include "CoreModules/ModuleGUI.h"
#include "Math/float3.h"

class GameObject;
class GUIInspector : public ModuleGUI {

public:

	GUIInspector(const char* _name);

	void Draw() override;
	void DrawGameObject();
	bool IsActive() const override { return active; };
	const char* GetName() const override { return name; }

	void ToggleActive();
	void SetFocusedGameObject(GameObject& focused) { focused_go = &focused; };

private:
	void UpdateTransform();
	void DrawDragFloat3(const char* name, float3& vector, float speed = 0.01f);

private:
	 GameObject* focused_go = nullptr;
};

