#pragma once

#include "Main/GameObject.h"

#include "Geometry/AABB.h"

#include <memory>

#define random() ((double) rand() / (RAND_MAX))

enum class SplitMethod { SAH, HLBVH, Middle, EqualCounts };

class BVHNode {

public:

	bool is_leaf = false;
	BVHNode* left = nullptr;
	BVHNode* right = nullptr;
	GameObject* go = nullptr;
	AABB aab_box;
	std::string name;

};

#ifdef _DEBUG
struct Trunk
{
	Trunk* prev;
	std::string str;

	Trunk(Trunk* prev, std::string str)
	{
		this->prev = prev;
		this->str = str;
	}
};
#endif // _DEBUG

class BVH{

	//void ConstructSAH(BVHNode *node, std::vector<std::unique_ptr<GameObject>>& orderedGameObjects);

public:

	BVH();
	~BVH();

	BVHNode*  GenerateBVH(std::vector<GameObject *>& orderedGameObjects, unsigned int size);

	void DebugBVH(BVHNode* node);

#ifdef _DEBUG
	void Print() { Inorder(_root, nullptr, false); }
#endif

	BVHNode* _root;

private:

	void DeleteRec(BVHNode* node);

#pragma region debug_functions

#ifdef _DEBUG
	void Inorder(BVHNode* root, Trunk* prev, bool isLeft);
#endif // _DEBUG

#pragma endregion debug_functions

};

