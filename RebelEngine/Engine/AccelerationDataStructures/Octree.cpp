#include "Octree.h"

#include "Utils/debugdraw.h"

unsigned int Octree::_maxObjPerNode = 5;

void OctreeNode::Shake() {

	for (auto it = _gos.begin(); it != _gos.end();) {

		GameObject* go = *it;
		AABB aabb; go->GetAABB(aabb);
		OBB obb = aabb.Transform(go->GetGlobalMatrix());

		bool intersects[8];
		bool keep_node = true;

		for (int i = 0; i < 8; i++) {
			intersects[i] = _children[i]._bounds.Intersects(obb.MinimalEnclosingAABB());
			keep_node = keep_node && intersects[i];
		}

		if (keep_node) ++it;
		else {
			it = _gos.erase(it);
			//Always 8 children
			for (int i = 0; i < 8; i++) {
				if (intersects[i]) _children[i].Insert(go);
			}
		}

	}

}

void OctreeNode::Split() {
	
	_children = std::vector<OctreeNode>(8);

	float3 center = _bounds.CenterPoint();
	float3 size = _bounds.Size();
		
	float3 new_center(center);
	float3 new_size(size * 0.5f);
	AABB new_box;

	new_box.SetFromCenterAndSize(center + float3(+size.x, -size.y, +size.z) * 0.25f, new_size);
	_children[0]._bounds = new_box;

	new_box.SetFromCenterAndSize(center + float3(+size.x, +size.y, +size.z) * 0.25f, new_size);
	_children[1]._bounds = new_box;

	new_box.SetFromCenterAndSize(center + float3(+size.x, -size.y, -size.z) * 0.25f, new_size);
	_children[2]._bounds = new_box;

	new_box.SetFromCenterAndSize(center + float3(+size.x, +size.y, -size.z) * 0.25f, new_size);
	_children[3]._bounds = new_box;

	new_box.SetFromCenterAndSize(center + float3(-size.x, -size.y, -size.z) * 0.25f, new_size);
	_children[4]._bounds = new_box;

	new_box.SetFromCenterAndSize(center + float3(-size.x, +size.y, -size.z) * 0.25f, new_size);
	_children[5]._bounds = new_box;

	new_box.SetFromCenterAndSize(center + float3(-size.x, -size.y, +size.z) * 0.25f, new_size);
	_children[6]._bounds = new_box;

	new_box.SetFromCenterAndSize(center + float3(-size.x, +size.y, +size.z) * 0.25f, new_size);
	_children[7]._bounds = new_box;
	
}

void OctreeNode::Insert(GameObject* go) {
	
	AABB aabb; go->GetAABB(aabb);
	OBB obb = aabb.Transform(go->GetGlobalMatrix());

	if (IsLeaf() && _gos.size() < Octree::_maxObjPerNode) {
		_gos.push_back(go);
	}
	else {

		if (IsLeaf()) {
			Split();
		}

		_gos.push_back(go);
		Shake();
	}
}

void OctreeNode::Remove(GameObject* go) {

	if (IsLeaf()) {
		auto it = std::find( _gos.begin(), _gos.end(), go );

		if (it != _gos.end()) { _gos.erase(it);	}

	}
	else {
		for (int i = 0; i < 8; ++i) {
			_children[i].Remove(go);
		}
	}
}

void OctreeNode::Update(GameObject* go) {
	Remove(go);
	Insert(go);
}

void Octree::Insert(OctreeNode* node, GameObject* go) { node->Insert(go); }
void Octree::Remove(OctreeNode* node, GameObject* go) { node->Remove(go); }
void Octree::Update(OctreeNode* node, GameObject* go) { node->Update(go); }

void Octree::DebugDraw(OctreeNode* node) {


	dd::aabb(node->_bounds.minPoint, node->_bounds.maxPoint, dd::colors::LightGreen);

	if (node->_children.size() != 0) {
		for (int i = 0; i < 8; i++) {
			DebugDraw(&(node->_children[i]));
		}
	}
	else {
		for (unsigned int i = 0; i < node->_gos.size(); i++) {
			AABB box; node->_gos[i]->GetAABB(box);
			OBB obb = box.Transform(node->_gos[i]->GetGlobalMatrix());
			dd::aabb(
				obb.MinimalEnclosingAABB().minPoint,
				obb.MinimalEnclosingAABB().maxPoint,
				dd::colors::Aquamarine
				);
		}
	}

}