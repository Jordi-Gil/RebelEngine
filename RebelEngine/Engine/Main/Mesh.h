#pragma once

#include <assimp/scene.h>
#include <vector>

#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include "json/json.h"
#include "Utils/RUUID.h"
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
		_uuid = RUUID::generate_uuid_v4();
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
		_uuid = RUUID::generate_uuid_v4();
	}

	~Mesh() {}

	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();

	void Draw(const std::vector<std::pair<unsigned int, TextureInformation>>& materials, const float4x4& model);

	void Clean();

	uint32_t GetMorton() const { return mortonCode; }

	bool FromJson(const Json::Value& value);
	bool WriteJsonFile();
	void SetName(const char* name) { _name = strdup(name); }
	const char* GetFilePath() { return _filePath; }

private:

	unsigned int VBO;
	unsigned int EBO;
	unsigned int VAO;
	unsigned int matIndex;
	unsigned int numVertices;
	unsigned int numIndices;
	unsigned int numFaces;

	Json::Value _vboValue;
	uint32_t mortonCode;
	char* _name;
	char _filePath[1024];
	std::string _uuid;
	AABB aabb;
	OBB obb;

private:

	bool LoadVBOFromJson(const Json::Value& value);
	bool LoadEBOFromJson(const Json::Value& value);
};

