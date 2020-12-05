#pragma once

#include "Main/GameObject.h"

#include "Geometry/AABB.h"

enum class SplitMethod { SAH, HLBVH, Middle, EqualCounts };

class BVHNode {

public:

	BVHNode* parent = nullptr;
	BVHNode* left = nullptr;
	BVHNode* right = nullptr;
	GameObject* go = nullptr;

	AABB box;

#ifdef _DEBUG
	const char* name = "";
#endif // _DEBUG

};

class BVH{

	//void ConstructSAH(BVHNode *node, std::vector<std::unique_ptr<GameObject>>& orderedGameObjects);

	BVHNode* GenerateBVH(std::vector<std::unique_ptr<GameObject>>& orderedGameObjects, unsigned int size);

	BVH(std::vector<std::unique_ptr<GameObject>> &orderedGameObjects);

	const SplitMethod split;
	BVHNode* root;
};

