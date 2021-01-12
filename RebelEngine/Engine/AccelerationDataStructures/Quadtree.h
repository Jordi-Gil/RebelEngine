#pragma once
#include "Main/GameObject.h"

#include <vector>
/*
class QuadTreeNode {

public:

	QuadTreeNode(AABB& box) : _node_box(box), _currentDepth(0) {}

	bool IsLeaf();

	int NumObjects();


private:

	std::vector<QuadTreeNode> _children;

	int _currentDepth;
	static int _maxDepth;
	static int _maxObjectsPerNode;
	AABB _node_box;

};
*/
class QuadTree {

public:

	QuadTree();

	//QuadTreeNode* _root;
};