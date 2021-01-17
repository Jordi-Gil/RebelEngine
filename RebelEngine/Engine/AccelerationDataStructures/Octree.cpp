#include "Octree.h"

#include "Main/Application.h"

#include "Utils/debugdraw.h"

#include "CoreModules/ModuleScene.h"

#include "Components/ComponentCamera.h"

unsigned int Octree::_maxObjPerNode = 50;
unsigned int Octree::_minSize = 100;


void OctreeNode::Shake() {

	for (auto it = _gos.begin(); it != _gos.end();) {

		GameObject* go = *it;
		OBB obb; go->GetOBB(obb);

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
	
	OBB obb; go->GetOBB(obb);

	if (IsLeaf() && _gos.size() < Octree::_maxObjPerNode &&
		_bounds.HalfSize().LengthSq() <= Octree::_minSize
		) {
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

	auto it = std::find( _gos.begin(), _gos.end(), go );

	if (it != _gos.end()) { _gos.erase(it);	}

	
	if(!IsLeaf()) {
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

	if (App->scene->_mainCamera->Intersects(node->_bounds)) {

		dd::aabb(node->_bounds.minPoint, node->_bounds.maxPoint, dd::colors::LightGreen);

		if (node->_children.size() != 0) {
			for (int i = 0; i < 8; i++) {
				DebugDraw(&(node->_children[i]));
			}
		}
		else {
			for (unsigned int i = 0; i < node->_gos.size(); i++) {
				OBB obb; node->_gos[i]->GetOBB(obb);
				dd::aabb(
					obb.MinimalEnclosingAABB().minPoint,
					obb.MinimalEnclosingAABB().maxPoint,
					dd::colors::Aquamarine
				);
			}
		}
	}
}