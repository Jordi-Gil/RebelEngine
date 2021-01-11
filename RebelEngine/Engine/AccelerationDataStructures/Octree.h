#pragma once

#include "Main/GameObject.h"

class OctreeNode
{

public:

	OctreeNode() : _children(0) {}
	~OctreeNode() {
		if (_children != 0) {
			delete[] _children;
		}
	}

public:

	void SplitTree(OctreeNode* node, int depth);

	void Insert(OctreeNode* node, GameObject* go);
	void Remove(OctreeNode* node, GameObject* go);
	void Update(OctreeNode* node, GameObject* go);

	AABB _bounds;
	OctreeNode* _children;

	std::vector<GameObject*> _gos;

};

class Octree {

public:

	Octree() { _root = new OctreeNode(); }
	~Octree() { delete _root; _root = nullptr; }

public:

	OctreeNode* _root = _root = nullptr;;

};
