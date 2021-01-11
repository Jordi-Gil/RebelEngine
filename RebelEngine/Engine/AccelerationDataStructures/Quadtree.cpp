#include "Quadtree.h"

#include <queue>

int QuadTreeNode::_maxDepth = 100;
int QuadTreeNode::_maxObjectsPerNode = 15;

bool QuadTreeNode::IsLeaf()
{
	return _children.size() == 0;
}

int QuadTreeNode::NumObjects() {
	Reset();

	int objectCount = _contents.size();
	for (int i = 0, size = _contents.size(); i < size; ++i) {
		_contents[i]->_flag = true;
	}

	std::queue<QuadTreeNode*> process;
	process.push(this);

	while (process.size() > 0) {
		QuadTreeNode* processing = process.back();

		if (!processing->IsLeaf()) {
			for (int i = 0, size = processing->_children.size(); i < size; ++i) {
				process.push(&processing->_children[i]);
			}
		}
		else {
			for (int i = 0, size = processing->_contents.size(); i < size; ++i) {
				if (!processing->_contents[i]->_flag) {
					objectCount += 1;
					processing->_contents[i]->_flag = true;
				}
			}
		}

		process.pop();
	}

	Reset();
	return objectCount;
}

void QuadTreeNode::Insert(QuadTreeData& data)
{
	if (!data._box.Intersects(_node_box)) {
		return; //Object does not fit
	}

	//If new object exceeds the limit of node, split the node
	if (IsLeaf() && _contents.size() + 1 > _maxObjectsPerNode) {
		Split();
	}

	if (IsLeaf()) {
		_contents.push_back(&data);
	}
	else {
		for (int i = 0, size = _children.size(); i < size; ++i) {
			_children[i].Insert(data);
		}
	}
}

void QuadTreeNode::Remove(QuadTreeData& data){

	if (IsLeaf()) {
		int removeIndex = -1;
		for (int i = 0, size = _contents.size(); i < size; ++i) {
			if (_contents[i]->_go == data._go) {
				removeIndex = i;
				break;
			}
		}

		if (removeIndex != -1) {
			_contents.erase(_contents.begin() + 1);
		}
	}
	else {
		for (int i = 0, size = _children.size(); i < size; ++i) {
			_children[i].Remove(data);
		}
	}

	Shake();
}

void QuadTreeNode::Update(QuadTreeData& data) {
	Remove(data);
	Insert(data);
}

void QuadTreeNode::Shake(){

	if (!IsLeaf()) {

	}

}

void QuadTreeNode::Split()
{
	if (_currentDepth + 1 >= _maxDepth) {
		return;
	}

	float3 size = _node_box.Size();
	float3 new_size(size.x*0.5, size.y, size.z*0.5);

	float3 center(_node_box.CenterPoint());

	AABB childAreas[4]{
		AABB(float3(center.x + size.x * 0.25f,center.y, center.z + size.z * 0.25f), new_size),
		AABB(float3(center.x + size.x * 0.25f,center.y, center.z - size.z * 0.25f), new_size),
		AABB(float3(center.x - size.x * 0.25f,center.y, center.z - size.z * 0.25f), new_size),
		AABB(float3(center.x - size.x * 0.25f,center.y, center.z + size.z * 0.25f), new_size)
	};
	
	for (int i = 0; i < 4; ++i) {
		_children.push_back(QuadTreeNode(childAreas[i]));
		_children[i]._currentDepth = _currentDepth + 1;
	}

	for (int i = 0, size = _contents.size(); i < size; ++i) {
		_children[i].Insert(*_contents[i]);
	}

	_contents.clear();

}

void QuadTreeNode::Reset() {
	if (IsLeaf()) {
		for (int i = 0, size = _contents.size(); i < size; ++i) {
			_contents[i]->_flag = false;
		}
	}
	else {
		for (int i = 0, size = _children.size(); i < size; ++i) {
			_children[i].Reset();
		}
	}
}

std::vector<QuadTreeData*> QuadTreeNode::Query(AABB& _box)
{
	return std::vector<QuadTreeData*>();
}
