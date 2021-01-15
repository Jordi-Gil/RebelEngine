#include "Mesh.h"

#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include "Math/float3x3.h"

#include "Application.h"

#include "CoreModules/ModuleEditorCamera.h"
#include "CoreModules/ModuleProgram.h"
#include "CoreModules/ModuleTexture.h"
#include "CoreModules/ModuleScene.h"

#include "Materials/MatStandard.h"

// method to seperate bits from a given integer 3 positions apart
inline uint32_t splitBy3(unsigned int a) {
	uint32_t x = a & 0x1fffff; // we only look at the first 21 bits
	x = (x | x << 16) & 0x1f0000ff0000ff; // shift left 32 bits, OR with self, and 00011111000000000000000011111111000000000000000011111111
	x = (x | x << 8) & 0x100f00f00f00f00f; // shift left 32 bits, OR with self, and 0001000000001111000000001111000000001111000000001111000000000000
	x = (x | x << 4) & 0x10c30c30c30c30c3; // shift left 32 bits, OR with self, and 0001000011000011000011000011000011000011000011000011000100000000
	x = (x | x << 2) & 0x1249249249249249;
	
	return x;
}

inline uint32_t mortonEncode_magicbits(unsigned int x, unsigned int y, unsigned int z) {
	uint32_t answer = 0;
	answer |= splitBy3(x) | splitBy3(y) << 1 | splitBy3(z) << 2;
	return answer;
}

void Mesh::LoadVBO(const aiMesh* mesh) {

	float max[3] = { FLT_MIN, FLT_MIN , FLT_MIN }, min[3] = { FLT_MAX , FLT_MAX , FLT_MAX };

	_matIndex = mesh->mMaterialIndex;

	glGenBuffers(1, &_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);

	unsigned int vertex_size = (sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 2); // Position / Normal / UV
	unsigned int vertices_size = vertex_size * mesh->mNumVertices; // Total size of Mesh
	
	glBufferData(GL_ARRAY_BUFFER, vertices_size, nullptr, GL_STATIC_DRAW); // 'Allocate' memory for all data set
	
	//Como assimp en mTextureCoords tiene varias dimensiones, para coger las coordenadas UV en 2D debemos acceder unicamente a la posicion 0
	float* data = (float*)(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	
	if (!data) LOG("glMapBuffer error", _ERROR);
	
	unsigned int pos = 0;
	unsigned int vertPos = 0;
	Json::Value jValue(Json::arrayValue);
	_vertices = std::vector<float3>(mesh->mNumVertices);

	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		//Position
		data[pos++] = mesh->mVertices[i].x; jValue.append(mesh->mVertices[i].x); 
		if (mesh->mVertices[i].x > max[0]) max[0] = mesh->mVertices[i].x;
		if (mesh->mVertices[i].x < min[0]) min[0] = mesh->mVertices[i].x;

		data[pos++] = mesh->mVertices[i].y; jValue.append(mesh->mVertices[i].y); 
		if (mesh->mVertices[i].y > max[1]) max[1] = mesh->mVertices[i].y;
		if (mesh->mVertices[i].y < min[1]) min[1] = mesh->mVertices[i].y;

		data[pos++] = mesh->mVertices[i].z; jValue.append(mesh->mVertices[i].z);
		if (mesh->mVertices[i].z > max[2]) max[2] = mesh->mVertices[i].z; 
		if (mesh->mVertices[i].z < min[2]) min[2] = mesh->mVertices[i].z;
		_vertices[vertPos++] = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		//Normal
		data[pos++] = mesh->mNormals[i].x; jValue.append(mesh->mNormals[i].x);
		data[pos++] = mesh->mNormals[i].y; jValue.append(mesh->mNormals[i].y);
		data[pos++] = mesh->mNormals[i].z; jValue.append(mesh->mNormals[i].z);

		//UV
		if (mesh->mTextureCoords[0]) {
			data[pos++] = mesh->mTextureCoords[0][i].x; jValue.append(mesh->mTextureCoords[0][i].x);
			data[pos++] = mesh->mTextureCoords[0][i].y; jValue.append(mesh->mTextureCoords[0][i].y);
		}
		else {
			data[pos++] = 0; jValue.append(0.0f);
			data[pos++] = 0; jValue.append(0.0f);
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	_numVertices = mesh->mNumVertices;
	_vboValue[0][JSON_TAG_VBO] = jValue; jValue.clear();
	_vboValue[0][JSON_TAG_VBO_SIZE] = pos;

	jValue.append(max[0]); jValue.append(max[1]); jValue.append(max[2]);
	_vboValue[0][JSON_TAG_VBO_MAX] = jValue; jValue.clear();

	jValue.append(min[0]); jValue.append(min[1]); jValue.append(min[2]);
	_vboValue[0][JSON_TAG_VBO_MIN] = jValue; jValue.clear();

	_aabb = AABB::AABB(vec(min[0], min[1], min[2]), vec(max[0], max[1], max[2]));
	vec centroid = _aabb.Centroid(); mortonCode = mortonEncode_magicbits(centroid[0], centroid[1], centroid[2]);
	//obb = OBB::OBB(aabb);
}

void Mesh::LoadEBO(const aiMesh* mesh) {

	glGenBuffers(1, &_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	unsigned int index_size = sizeof(unsigned) * mesh->mNumFaces * 3;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
	unsigned* indices = (unsigned*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
	
	if(!indices) LOG("glMapBuffer error", _ERROR);

	Json::Value jValue(Json::arrayValue);
	
	for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
		for (uint j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
			*(indices++) = mesh->mFaces[i].mIndices[j];
			jValue.append(mesh->mFaces[i].mIndices[j]);
			_indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}
	
	_vboValue[0][JSON_TAG_EBO] = jValue;

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	
	_numFaces = mesh->mNumFaces;
	_numIndices = mesh->mNumFaces * 3;

}

void Mesh::CreateVAO() {

	glGenVertexArrays(1, &_VAO);
	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

	GLsizei stride = sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 2;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);			
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);					
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));

	glBindVertexArray(0);

}

void Mesh::Draw(Material* material, const float4x4& model) {

	MatStandard* mat = static_cast<MatStandard*>(material);

	unsigned int program = App->program->GetMainProgram();

	float4x4 view; App->editorCamera->GetMatrix(matrix_type::VIEW_MATRIX, view);
	float4x4 projection; App->editorCamera->GetMatrix(matrix_type::PROJECTION_MATRIX, projection);

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*) &model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*) &view);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, (const float*) &projection);
	glUniform1i(glGetUniformLocation(program, "textureEnabled"), true);
	glUniform1i(glGetUniformLocation(program, "mytexture"), 0);

	glBindVertexArray(_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mat->GetAlbedoMap());

	glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);

}

void Mesh::Clean() {
	LOG(_INFO, "Deleting buffers");

	glDeleteBuffers(1, &_VBO);
	glDeleteBuffers(1, &_EBO);
	glDeleteBuffers(1, &_VAO);
}

bool Mesh::WriteJsonFile(){

	_vboValue[0][JSON_TAG_NAME] = _name;
	_vboValue[0][JSON_TAG_UUID] = _uuid;
	_vboValue[0][JSON_TAG_MATERIAL_INDEX] = _matIndex;
	_vboValue[0][JSON_TAG_NUM_VERTICES] = _numVertices;
	_vboValue[0][JSON_TAG_NUM_FACES] = _numFaces;
	_vboValue[0][JSON_TAG_NUM_INDICES] = _numIndices;

	sprintf(_filePath, "%s%s%s", DEFAULT_MESH_PATH, _name, DEFAULT_MESH_EXT);

	Json::StyledWriter wr;
	std::ofstream ofs(_filePath, std::ios_base::binary);
	std::string st = wr.write(_vboValue);
	ofs.write(st.c_str(), st.size());
	ofs.close();

	return true;
}

bool Mesh::FromJson(const Json::Value& value) {

	if (!value.isNull()) 
	{
		_name = _strdup(value[JSON_TAG_NAME].asCString());
		_uuid = value[JSON_TAG_UUID].asString();
		_matIndex = value[JSON_TAG_MATERIAL_INDEX].asInt();
		_numVertices = value[JSON_TAG_NUM_VERTICES].asInt();
		_numFaces = value[JSON_TAG_NUM_FACES].asInt();
		_numIndices = value[JSON_TAG_NUM_INDICES].asInt();

		LoadVBOFromJson(value);
		LoadEBOFromJson(value);
		CreateVAO();

	}
	else 
	{
		return false;
	}
	return true;
}

bool Mesh::LoadVBOFromJson(const Json::Value& value)  {

	if (!value.isNull()) 
	{
		glGenBuffers(1, &_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, _VBO);

		unsigned int vertex_size = (sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 2); // Position / Normal / UV
		unsigned int vertices_size = vertex_size * _numVertices; // Total size of Mesh

		glBufferData(GL_ARRAY_BUFFER, vertices_size, nullptr, GL_STATIC_DRAW); // 'Allocate' memory for all data set

		//Como assimp en mTextureCoords tiene varias dimensiones, para coger las coordenadas UV en 2D debemos acceder unicamente a la posicion 0
		float* data = (float*)(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

		if (!data) LOG("glMapBuffer error", _ERROR);

		for (unsigned i = 0; i < value[JSON_TAG_VBO_SIZE].asUInt(); i+=8)
		{ 
			data[i] = value[JSON_TAG_VBO][i].asFloat();
			data[i+1] = value[JSON_TAG_VBO][i+1].asFloat();
			data[i+2] = value[JSON_TAG_VBO][i+2].asFloat();
			data[i+3] = value[JSON_TAG_VBO][i+3].asFloat();
			data[i+4] = value[JSON_TAG_VBO][i+4].asFloat();
			data[i+5] = value[JSON_TAG_VBO][i+5].asFloat();
			data[i+6] = value[JSON_TAG_VBO][i+6].asFloat();
			data[i+7] = value[JSON_TAG_VBO][i+7].asFloat();

			_vertices.push_back(float3(data[i], data[i + 1], data[i + 2]));
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);

		_aabb = AABB::AABB(vec(value[JSON_TAG_VBO_MIN][0].asFloat(), value[JSON_TAG_VBO_MIN][1].asFloat(), value[JSON_TAG_VBO_MIN][2].asFloat()), 
						  vec(value[JSON_TAG_VBO_MAX][0].asFloat(), value[JSON_TAG_VBO_MAX][1].asFloat(), value[JSON_TAG_VBO_MAX][2].asFloat()));
		vec centroid = _aabb.Centroid(); mortonCode = mortonEncode_magicbits(centroid[0], centroid[1], centroid[2]);
	}
	else  {
		return false;
	}

	return true;
}

bool Mesh::LoadEBOFromJson(const Json::Value& value) {
	
	if (!value.isNull())
	{
		glGenBuffers(1, &_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
		unsigned int index_size = sizeof(unsigned) * _numFaces * 3;

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
		unsigned* indices = (unsigned*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

		if (!indices) LOG(_ERROR, "glMapBuffer error");
		int aux = value[JSON_TAG_EBO].size();
		_indices = std::vector<unsigned>(aux);
		for (unsigned i = 0; i < aux; ++i) {
			indices[i] = value[JSON_TAG_EBO][i].asUInt();
			_indices[i] = value[JSON_TAG_EBO][i].asUInt();
		}
		
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	}
	else
	{
		return false;
	}
	return true;
}