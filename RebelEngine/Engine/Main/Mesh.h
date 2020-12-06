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
		VBO(0), EBO(0), VAO(0), matIndex(0), 
		numVertices(0), numIndices(0), numFaces(0)
	{
		aabb.SetNegativeInfinity();
		obb.SetNegativeInfinity();
	}

	Mesh(Mesh&& _mesh) {

		VBO = _mesh.VAO;
		EBO = _mesh.EBO;
		VAO = _mesh.VAO;
		matIndex = _mesh.matIndex;
		numVertices = _mesh.numVertices;
		numIndices = _mesh.numIndices;
		numFaces = _mesh.numFaces;
		aabb = aabb;
		obb = obb;
	}

	~Mesh() {}

	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();

	void Draw(const std::vector<std::pair<unsigned int, TextureInformation>>& materials);

	void Clean();

	uint32_t GetMorton() const { return mortonCode; }

private:

	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;
	unsigned int matIndex;
	unsigned int numVertices;
	unsigned int numIndices;
	unsigned int numFaces;

	uint32_t mortonCode;

	AABB aabb;
	OBB obb;
};

