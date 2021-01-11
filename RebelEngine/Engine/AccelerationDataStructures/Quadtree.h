#pragma once
#include "Main/GameObject.h"

#include <vector>

struct QuadTreeData {

	GameObject* _go;
	AABB _box;
	bool _flag;

	inline QuadTreeData(GameObject* go, const AABB& box) {
		_go = go;
		_box = box;
		_flag = false;
	}

};

class QuadTreeNode {

public:

	QuadTreeNode(AABB& box) : _node_box(box), _currentDepth(0) {}

	bool IsLeaf();

	int NumObjects();

	void Insert(QuadTreeData& data);
	void Remove(QuadTreeData& data);
	void Update(QuadTreeData& data);

	void Shake();
	void Split();
	void Reset();
	std::vector<QuadTreeData*> Query(AABB& _box);

private:

	std::vector<QuadTreeNode> _children;
	std::vector<QuadTreeData*> _contents;

	int _currentDepth;
	static int _maxDepth;
	static int _maxObjectsPerNode;
	AABB _node_box;

};

class QuadTree {

public:

	QuadTree();

	QuadTreeNode* _root;
};