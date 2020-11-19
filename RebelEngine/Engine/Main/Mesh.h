#pragma once

#include <assimp/scene.h>
#include <Math/float2.h>
#include <Math/float3.h>

#include <vector>

class Mesh
{

public: 
		
	Mesh() : VBO(0), EBO(0), VAO(0), matIndex(0), numVertices(0), numIndices(0) {}
	~Mesh();

	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();

	void Draw(const std::vector<unsigned int>& materials);

	void Clean();

private:
	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;
	unsigned int matIndex;
	unsigned int numVertices;
	unsigned int numIndices;
};

