#pragma once

#include <assimp/scene.h>
#include <vector>

#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

struct TextureInformation;

class Mesh {

friend class GUIInspector;

public: 
		
	Mesh() : 
		_VBO(0), _EBO(0), _VAO(0), _matIndex(0), 
		_numVertices(0), _numIndices(0), _numFaces(0)
	{
		_aabb.SetNegativeInfinity();
		_obb.SetNegativeInfinity();
	}

	Mesh(Mesh&& _mesh) {

		_VBO = _mesh._VAO;
		_EBO = _mesh._EBO;
		_VAO = _mesh._VAO;
		_matIndex = _mesh._matIndex;
		_numVertices = _mesh._numVertices;
		_numIndices = _mesh._numIndices;
		_numFaces = _mesh._numFaces;
		_aabb = _mesh._aabb;
		_obb = _mesh._obb;
	}

	~Mesh() {}

	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();

	void Draw(const std::vector<std::pair<unsigned int, TextureInformation>>& materials, const float4x4& model);

	void Clean();

	uint32_t GetMorton() const { return _mortonCode; }
	void GetAABB(AABB& aabb) const { aabb = _aabb; }

private:

	unsigned int _VBO;
	unsigned int _EBO;
	unsigned int _VAO;
	unsigned int _matIndex;
	unsigned int _numVertices;
	unsigned int _numIndices;
	unsigned int _numFaces;

	uint32_t _mortonCode;

	AABB _aabb;
	OBB _obb;
};

