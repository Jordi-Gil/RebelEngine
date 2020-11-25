#pragma once

#include <assimp/scene.h>
#include <vector>

struct TextureInformation;

class Mesh {

friend class GUIInspector;

public: 
		
	Mesh() : VBO(0), EBO(0), VAO(0), matIndex(0), numVertices(0), numIndices(0), numFaces(0) {}
	~Mesh();

	void LoadVBO(const aiMesh* mesh, float max[3], float min[3]);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();

	void Draw(const std::vector<std::pair<unsigned int, TextureInformation>>& materials);

	void Clean();

private:
	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;
	unsigned int matIndex;
	unsigned int numVertices;
	unsigned int numIndices;
	unsigned int numFaces;
};

