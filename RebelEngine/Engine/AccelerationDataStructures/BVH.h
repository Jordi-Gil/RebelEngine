#pragma once

#include "Main/GameObject.h"

#include "Geometry/AABB.h"

enum class SplitMethod { SAH, HLBVH, Middle, EqualCounts };

class BVHNode {

public:

	BVHNode* parent = nullptr;
	BVHNode* left = nullptr;
	BVHNode* right = nullptr;
	//GameObject* go = nullptr;

	AABB box;
	bool is_leaf = false;

//#ifdef _DEBUG
	const char* name = "";
//#endif // _DEBUG

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

	BVH(std::vector<GameObject*>& orderedGameObjects);
	~BVH();
	void Print() { Inorder(_root, nullptr, false); }

private:

	BVHNode* GenerateBVH(std::vector<GameObject *>& orderedGameObjects, unsigned int size);
	BVHNode* _root = nullptr;

#pragma region debug_functions

#ifdef _DEBUG
	void Inorder(BVHNode* root, Trunk* prev, bool isLeft);
#endif // _DEBUG

#pragma endregion debug_functions

};

