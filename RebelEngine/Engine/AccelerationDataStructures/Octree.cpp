#include "Octree.h"

void OctreeNode::SplitTree(OctreeNode* node, int depth) {

	if (depth-- <= 0) {
		return;
	}

	//If the current node has no childre, split into 8 child nodes
	if (node->_children == 0) {
		node->_children = new OctreeNode[8];

		float3 c = node->_bounds.CenterPoint();
		float3 e = node->_bounds.Size() * 0.5f;

		node->_children[0]._bounds =
			AABB(c + float3(-e.x, +e.y, -e.z), e);
		node->_children[1]._bounds =
			AABB(c + float3(+e.x, +e.y, -e.z), e);
		node->_children[2]._bounds =
			AABB(c + float3(-e.x, +e.y, +e.z), e);
		node->_children[3]._bounds =
			AABB(c + float3(+e.x, +e.y, +e.z), e);
		node->_children[4]._bounds =
			AABB(c + float3(-e.x, -e.y, -e.z), e);
		node->_children[5]._bounds =
			AABB(c + float3(+e.x, -e.y, -e.z), e);
		node->_children[6]._bounds =
			AABB(c + float3(-e.x, -e.y, +e.z), e);
		node->_children[7]._bounds =
			AABB(c + float3(+e.x, -e.y, +e.z), e);
	}

	if (node->_children != 0 && node->_gos.size() > 0) {
		for (int i = 0; i < 8; ++i) {
			for (int j = 0, size = node->_gos.size(); j < size; ++j) {
				AABB box;  node->_gos[j]->GetAABB(box);
				OBB obb = box.Transform(node->_gos[i]->GetGlobalMatrix());
				if (node->_children[i]._bounds.Intersects(obb)) {
					node->_children[i]._gos.push_back(node->_gos[j]);
				}
			}
		}

		node->_gos.clear();

		for (int i = 0; i < 8; ++i) {
			SplitTree(&(node->_children[i]), depth);
		}
	}
}

void OctreeNode::Insert(OctreeNode* node, GameObject* go) {
	
	AABB aabb; go->GetAABB(aabb);
	OBB obb = aabb.Transform(go->GetGlobalMatrix());

	if (node->_bounds.Intersects(obb)) {
		if(node->_children == 0){
			node->_gos.push_back(go);
		}
		else {
			for (int i = 0; i < 8; ++i) {
				Insert(&(node->_children[i]), go);
			}
		}
	}
}

void OctreeNode::Remove(OctreeNode* node, GameObject* go) {

	if (node->_children == 0) {
		auto it = std::find(
				node->_gos.begin(), 
				node->_gos.end(),
				go
		);

		if (it != node->_gos.end()) {
			node->_gos.erase(it);
		}

	}
	else {
		for (int i = 0; i < 8; ++i) {
			Remove(&(node->_children[i]), go);
		}
	}
}

void OctreeNode::Update(OctreeNode* node, GameObject* go) {
	Remove(node, go);
	Insert(node, go);
}