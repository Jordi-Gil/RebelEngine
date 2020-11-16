#include "Mesh.h"

#include "Utils/Console.h"

#include "CoreModules/ModuleEditorCamera.h"
#include "CoreModules/ModuleProgram.h"

#include "Application.h"

#include <GL/glew.h>

void Mesh::LoadVBO(const aiMesh* mesh) {

	matIndex = mesh->mMaterialIndex;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	unsigned int vertex_size = (sizeof(float) * 3 + sizeof(float) * 2); // Position / UV
	unsigned int vertices_size = vertex_size * mesh->mNumVertices; // Total size of Mesh
	
	glBufferData(GL_ARRAY_BUFFER, vertices_size, nullptr, GL_STATIC_DRAW); // 'Allocate' memory for all data set
	
	//Como assimp en mTextureCoords tiene varias dimensiones, para coger las coordenadas UV en 2D debemos acceder unicamente a la posicion 0
	float* data = (float*)(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	
	if (!data) LOG("glMapBuffer error", _ERROR);
	unsigned int pos = 0;
	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		//Position
		data[pos++] = mesh->mVertices[i].x;
		data[pos++] = mesh->mVertices[i].y;
		data[pos++] = mesh->mVertices[i].z;
		//UV
		data[pos++] = mesh->mTextureCoords[0][i].x;
		data[pos++] = mesh->mTextureCoords[0][i].y;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	numVertices = mesh->mNumVertices;
}

void Mesh::LoadEBO(const aiMesh* mesh) {

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	unsigned int index_size = sizeof(unsigned) * mesh->mNumFaces * 3;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
	unsigned* indices = (unsigned*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
	
	if(!indices) LOG("glMapBuffer error", _ERROR);

	for (unsigned i = 0; i < mesh->mNumFaces; ++i)
	{
		assert(mesh->mFaces[i].mNumIndices == 3); // note: assume triangles = 3 indices per face
		*(indices++) = mesh->mFaces[i].mIndices[0];
		*(indices++) = mesh->mFaces[i].mIndices[1];
		*(indices++) = mesh->mFaces[i].mIndices[2];
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	
	numIndices = mesh->mNumFaces * 3;

}

void Mesh::CreateVAO() {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3 + sizeof(float) * 2, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 3 + sizeof(float) * 2, (void*)(sizeof(float) * 3));

	glBindVertexArray(0);

}

void Mesh::Draw(const std::vector<unsigned int> &materials) {

	unsigned int program = App->program->GetMainProgram();

	float4x4 model = float4x4::identity;
	float4x4 view; App->editorCamera->GetMatrix(VIEW_MATRIX, view);
	float4x4 projection; App->editorCamera->GetMatrix(PROJECTION_MATRIX, projection);

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*) &model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*) &view);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, (const float*) &projection);
	glUniform1i(glGetUniformLocation(program, "textureEnabled"), true);
	glUniform1i(glGetUniformLocation(program, "mytexture"), 0);

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, materials[matIndex]);

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);

}

void Mesh::Clean() {

	LOG("Deleting buffers",_INFO);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VAO);

}