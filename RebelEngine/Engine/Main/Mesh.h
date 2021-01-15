#pragma once

#include <assimp/scene.h>
#include <vector>

#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

#include "json/json.h"

#include "Materials/Material.h"

#include "Utils/RUUID.h"

struct TextureInformation;

class Mesh {

friend class GUIInspector;

public: 
		
	Mesh() : _VBO(0), _EBO(0), _VAO(0), _matIndex(0), _numVertices(0), _numIndices(0), _numFaces(0) {
		_aabb.SetNegativeInfinity();
		_obb.SetNegativeInfinity();
		_uuid = RUUID::generate_uuid_v4();
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
		_uuid = RUUID::generate_uuid_v4();
	}

	~Mesh() {}

	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();

	void Draw(Material* material, const float4x4& model);

	void Clean();

	uint32_t GetMorton() const { return _mortonCode; }
	void GetAABB(AABB& aabb) const { aabb = _aabb; }
	const char* GetFilePath() { return _filePath; }

	void SetName(const char* name) { _name = _strdup(name); }
	void SetFilePath(const char* filePath) { sprintf_s(_filePath, "%s", filePath); }

	bool FromJson(const Json::Value& value);
	bool WriteJsonFile();

public:
	std::vector<float3> _vertices;
	std::vector<unsigned> _indices;

private:

	bool LoadVBOFromJson(const Json::Value& value);
	bool LoadEBOFromJson(const Json::Value& value);

private:

	friend class ComponentMeshRenderer;

	unsigned int _VBO;
	unsigned int _EBO;
	unsigned int _VAO;
	unsigned int _matIndex;
	unsigned int _numVertices;
	unsigned int _numIndices;
	unsigned int _numFaces;

	uint32_t _mortonCode;

	char* _name;
	char _filePath[1024];
	std::string _uuid;

	AABB _aabb;
	OBB _obb;

	Json::Value _meshValue;
	
};

