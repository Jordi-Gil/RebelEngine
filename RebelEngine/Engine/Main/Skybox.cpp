#include "Skybox.h"

#include "Main/Application.h"

#include "CoreModules/ModuleTexture.h"
#include "CoreModules/ModuleProgram.h"
#include "CoreModules/ModuleEditorCamera.h"

#include <Math/float4x4.h>

#include <GL/glew.h>

Skybox::Skybox() {

    float skyboxVertices[108] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    skyTexture = App->texturer->loadCubeMap("Assets/Textures/Cubemaps/abandoned/");

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

}

Skybox::~Skybox() {
    glDeleteTextures(1, &skyTexture);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VAO);
}

void Skybox::Draw() {

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    unsigned int program = App->program->GetSkyboxProgram();

    float4x4 view; App->editorCamera->GetMatrix(matrix_type::VIEW_MATRIX, view);
    float4x4 projection; App->editorCamera->GetMatrix(matrix_type::PROJECTION_MATRIX, projection);

    glUseProgram(program);

    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, (const float*)&projection);
    glUniform1i(glGetUniformLocation(program, "skybox"), 0);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

}
