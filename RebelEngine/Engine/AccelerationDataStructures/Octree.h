#pragma once

#include "Main/GameObject.h"

class OctreeNode
{

public:

	OctreeNode() {}
	~OctreeNode() {}

public:

	bool IsLeaf() { return _children.size() == 0; }

	void Shake();
	void Split();

	void Insert(GameObject *go);
	void Remove(GameObject *go);
	void Update(GameObject *go);

	AABB _bounds;

	std::vector<OctreeNode> _children;
	std::vector<GameObject*> _gos;

};

class Octree {

public:

	Octree() { _root = new OctreeNode(); }
	~Octree() { delete _root; _root = nullptr; }

	void DebugDraw(OctreeNode* node);

	void Insert(OctreeNode* node, GameObject* go);
	void Remove(OctreeNode* node, GameObject* go);
	void Update(OctreeNode* node, GameObject* go);

public:

	OctreeNode* _root = _root = nullptr;

	static int _maxObjPerNode;

};
