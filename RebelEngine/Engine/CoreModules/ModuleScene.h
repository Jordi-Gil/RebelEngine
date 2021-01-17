#pragma once
#include "Module.h"

#include "Main/Pool.h"

#include <memory>
#include <vector>

#include "Math/float3.h"

#include "json/json.h"

class Octree;
class OctreeNode;
class Skybox;
class GameObject;
class ComponentCamera;

enum Rebel_FrustumMask {
	NO_FRUSTUM  = 1,
	LINEAR_AABB = 1 << 1,
	OCTREE		= 1 << 2
};

class ModuleScene : public Module {

public:

	ModuleScene();
	ModuleScene(const Json::Value& value);
	~ModuleScene();

	bool Init() override;
	bool Start() override;

	update_status PreUpdate() override;

	void Draw();

	void SetMask(Rebel_FrustumMask mask) { _mask = mask; }
	void SetMainCamera(ComponentCamera& mainCamera);

	void UpdateMinMax(float3 min, float3 max);

	bool Save();
	bool Load();
	bool ToJson(Json::Value& value, int pos);
	bool FromJson(const Json::Value& value);
	void TogglePlay();
	bool IsBusy() { return _isSaving && _isLoading; };
	bool IsPlaying() { return _isPlaying; }

private:

	void IterateRoot(GameObject &go);
	void DrawRecursive(GameObject &go);
	void DrawFrustumOutput();

	void FrustumCulling(OctreeNode* node);
	void FrustumCulling();

	
public:

	Pool<GameObject> _poolGameObjects;
	std::unique_ptr<GameObject> _root;
	Skybox* _skybox;
	Octree* _octree = nullptr;

private:

	bool _frustum = false;

	int _mask = NO_FRUSTUM;

	GameObject* _goSelected = nullptr;
	ComponentCamera* _mainCamera = nullptr;

	float3 _max = float3(FLT_MIN, FLT_MIN, FLT_MIN);
	float3 _min = float3(FLT_MAX, FLT_MAX, FLT_MAX);

	std::vector<GameObject*> _objects;
	std::vector<GameObject *> _objectsToDraw;

	bool _isPlaying = false;
	bool _isSaving = false;
	bool _isLoading = false;
};

