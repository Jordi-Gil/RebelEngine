#include "Mesh.h"

#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include "Math/float3x3.h"

#include "Application.h"
#include "GameObject.h"

#include "CoreModules/ModuleEditorCamera.h"
#include "CoreModules/ModuleProgram.h"
#include "CoreModules/ModuleTexture.h"
#include "CoreModules/ModuleScene.h"

#include "Components/ComponentLight.h"


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
	_meshValue[0][JSON_TAG_VBO] = jValue; jValue.clear();
	_meshValue[0][JSON_TAG_VBO_SIZE] = pos;

	jValue.append(max[0]); jValue.append(max[1]); jValue.append(max[2]);
	_meshValue[0][JSON_TAG_VBO_MAX] = jValue; jValue.clear();

	jValue.append(min[0]); jValue.append(min[1]); jValue.append(min[2]);
	_meshValue[0][JSON_TAG_VBO_MIN] = jValue; jValue.clear();

	_aabb = AABB::AABB(vec(min[0], min[1], min[2]), vec(max[0], max[1], max[2]));
	vec centroid = _aabb.Centroid(); _mortonCode = mortonEncode_magicbits(centroid[0], centroid[1], centroid[2]);

}

bool Mesh::LoadEBO(const aiMesh* mesh) {

	glGenBuffers(1, &_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	unsigned int index_size = sizeof(unsigned) * mesh->mNumFaces * 3;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
	unsigned* indices = (unsigned*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
	
	if(!indices) LOG("glMapBuffer error", _ERROR);

	Json::Value jValue(Json::arrayValue);

	bool correct = true;
	for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
		if (mesh->mFaces[i].mNumIndices != 3) correct = false;
		for (uint j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
			*(indices++) = mesh->mFaces[i].mIndices[j];
			jValue.append(mesh->mFaces[i].mIndices[j]);
			_indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}
	
	_meshValue[0][JSON_TAG_EBO] = jValue;

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	
	_numFaces = mesh->mNumFaces;
	_numIndices = mesh->mNumFaces * 3;

	return correct;

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

void Mesh::Render(Material* material, const float4x4& model) {

	std::vector<GameObject*> lights; App->scene->GetLights(lights);

	MatStandard* mat = static_cast<MatStandard*>(material);

	unsigned int program = App->program->GetMainProgram();

	float4x4 view; App->editorCamera->GetMatrix(matrix_type::VIEW_MATRIX, view);
	float4x4 projection; App->editorCamera->GetMatrix(matrix_type::PROJECTION_MATRIX, projection);

	glUseProgram(program);
	float3 camPos = App->editorCamera->GetPosition();
	glUniform3f(glGetUniformLocation(program, "cameraPosition"), camPos.x, camPos.y, camPos.z);
	glUniformMatrix4fv(glGetUniformLocation(program, "matGeo"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "matView"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "matProj"), 1, GL_TRUE, (const float*)&projection);

	bool checkTexture;
	checkTexture = mat->_maps[0].gl_id != 0 ? true : false;
	glUniform1ui(glGetUniformLocation(program, "specular_material.has_diffuse_map"), checkTexture);
	checkTexture = mat->_maps[1].gl_id != 0 ? true : false;
	glUniform1ui(glGetUniformLocation(program, "specular_material.has_specular_map"), checkTexture);
	checkTexture = mat->_maps[2].gl_id != 0 ? true : false;
	glUniform1ui(glGetUniformLocation(program, "specular_material.has_normal_map"), checkTexture);

	glUniform1i(glGetUniformLocation(program, "specular_material.diffuse_map"), 0);
	glUniform1i(glGetUniformLocation(program, "specular_material.specular_map"), 1);
	glUniform1i(glGetUniformLocation(program, "specular_material.normal_map"), 2);

	float3 diffuse_color; mat->GetColor(diffuse_color);
	glUniform3f(glGetUniformLocation(program, "specular_material.diffuse_color"), diffuse_color.x, diffuse_color.y, diffuse_color.z);
	glUniform3f(glGetUniformLocation(program, "specular_material.specular_color"), 0.8, 0.8, 0.8);

	//Ambient Light
	glUniform3f(glGetUniformLocation(program, "lights.ambientlight.ambient"), 0.8, 0.8, 0.8);
	unsigned int num_points = 0, num_spots = 0;
	for (int i = 0; i < lights.size(); i++) {
		ComponentLight* light = (ComponentLight*)lights[i]->GetComponent(type_component::LIGHT);

		switch (light->GetLightType())
		{
		case light_type::DIRECTIONAL_LIGHT: {
			glUniform1f(glGetUniformLocation(program, "lights.directionalLight.intensity"), light->GetIntensity());
			float4x4 global = light->GetOwner()->GetGlobalMatrix();
			float3 forward = global.WorldZ();
			glUniform3f(glGetUniformLocation(program, "lights.directionalLight.direction"), forward.x, forward.y, forward.z);
			float3 color = light->GetColor();
			glUniform3f(glGetUniformLocation(program, "lights.directionalLight.baseLight.diffuse"), color.x, color.y, color.z);
			glUniform3f(glGetUniformLocation(program, "lights.directionalLight.baseLight.specular"), 0.5f, 0.5f, 0.5f);
			break;
		}
		case light_type::POINT_LIGHT: {
			char location[1024];
			sprintf_s(location, 1024, "lights.pointLights[%d].position",num_points);
			float3 position = light->GetOwner()->GetGlobalMatrix().TranslatePart();
			glUniform3f(glGetUniformLocation(program, location), position.x, position.y, position.z);
			sprintf_s(location, 1024, "lights.pointLights[%d].intensity", num_points);
			glUniform1f(glGetUniformLocation(program, location), light->GetIntensity());
			sprintf_s(location, 1024, "lights.pointLights[%d].att.constant", num_points);
			glUniform1f(glGetUniformLocation(program, location), light->GetConstantAtt());
			sprintf_s(location, 1024, "lights.pointLights[%d].att.linear", num_points);
			glUniform1f(glGetUniformLocation(program, location), light->GetLinearAtt());
			sprintf_s(location, 1024, "lights.pointLights[%d].att.quadratic", num_points);
			glUniform1f(glGetUniformLocation(program, location), light->GetQuadraticAtt());
			float3 color = light->GetColor();
			sprintf_s(location, 1024, "lights.pointLights[%d].baseLight.diffuse", num_points);
			glUniform3f(glGetUniformLocation(program, location), color.x, color.y, color.z);
			sprintf_s(location, 1024, "lights.pointLights[%d].baseLight.specular", num_points);
			glUniform3f(glGetUniformLocation(program, location), 0.8, 0.8, 0.8);
			++num_points;
			break;
		}
		case light_type::SPOT_LIGHT: {
			char location[1024];
			sprintf_s(location, 1024, "lights.spotLights[%d].position", num_spots);
			float3 position = light->GetOwner()->GetGlobalMatrix().TranslatePart();
			glUniform3f(glGetUniformLocation(program, location), position.x, position.y, position.z);

			sprintf_s(location, 1024, "lights.spotLights[%d].aim", num_spots);
			float4x4 global = light->GetOwner()->GetGlobalMatrix();
			float3 forward = global.WorldZ();
			glUniform3f(glGetUniformLocation(program, location), forward.x, forward.y, forward.z);

			sprintf_s(location, 1024, "lights.spotLights[%d].intensity", num_spots);
			glUniform1f(glGetUniformLocation(program, location), light->GetIntensity());

			sprintf_s(location, 1024, "lights.spotLights[%d].innerAngle", num_spots);
			glUniform1f(glGetUniformLocation(program, location), DegToRad(light->GetInnerAngle()));
			sprintf_s(location, 1024, "lights.spotLights[%d].outterAngle", num_spots);
			glUniform1f(glGetUniformLocation(program, location), DegToRad(light->GetOutterAngle()));

			sprintf_s(location, 1024, "lights.spotLights[%d].att.constant", num_spots);
			glUniform1f(glGetUniformLocation(program, location), light->GetConstantAtt());
			sprintf_s(location, 1024, "lights.spotLights[%d].att.linear", num_spots);
			glUniform1f(glGetUniformLocation(program, location), light->GetLinearAtt());
			sprintf_s(location, 1024, "lights.spotLights[%d].att.quadratic", num_spots);
			glUniform1f(glGetUniformLocation(program, location), light->GetQuadraticAtt());
			float3 color = light->GetColor();
			sprintf_s(location, 1024, "lights.spotLights[%d].baseLight.diffuse", num_spots);
			glUniform3f(glGetUniformLocation(program, location), color.x, color.y, color.z);
			sprintf_s(location, 1024, "lights.spotLights[%d].baseLight.specular", num_spots);
			glUniform3f(glGetUniformLocation(program, location), 0.8, 0.8, 0.8);
			++num_spots;
		}
		}
	}

	glUniform1ui(glGetUniformLocation(program, "lights.num_points"), num_points);
	glUniform1ui(glGetUniformLocation(program, "lights.num_spots"), num_spots);

	glBindVertexArray(_VAO);

	for (int i = 0; i < 3; i++) {

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, mat->_maps[i].gl_id);
	}

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

	_meshValue[0][JSON_TAG_NAME] = _name;
	_meshValue[0][JSON_TAG_UUID] = _uuid;
	_meshValue[0][JSON_TAG_MATERIAL_INDEX] = _matIndex;
	_meshValue[0][JSON_TAG_NUM_VERTICES] = _numVertices;
	_meshValue[0][JSON_TAG_NUM_FACES] = _numFaces;
	_meshValue[0][JSON_TAG_NUM_INDICES] = _numIndices;

	char filename[FILENAME_MAX];
	sprintf(filename, "%s%s", _name, _uuid.c_str());
	sprintf(_filePath, "%s%s%s", DEFAULT_MESH_PATH, filename, DEFAULT_MESH_EXT);

	Json::StyledWriter wr;
	std::ofstream ofs(_filePath, std::ios_base::binary);
	std::string st = wr.write(_meshValue);
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
		vec centroid = _aabb.Centroid(); _mortonCode = mortonEncode_magicbits(centroid[0], centroid[1], centroid[2]);
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