#include "ModuleProgram.h"

#include "Main/Application.h"

#include <fstream>
#include <GL/glew.h>

ModuleProgram::ModuleProgram() {}

bool ModuleProgram::Start() {

	GLuint vertexSahder, fragmentShader;

	vertexSahder = App->program->CompileShader(GL_VERTEX_SHADER, (App->program->readFile("Shaders/PBR_Phong.vert")).c_str());
	fragmentShader = App->program->CompileShader(GL_FRAGMENT_SHADER, (App->program->readFile("Shaders/PBR_Phong.frag")).c_str());
	mainProgram = App->program->CreateProgram(vertexSahder, fragmentShader);

	vertexSahder = App->program->CompileShader(GL_VERTEX_SHADER, (App->program->readFile("Shaders/skybox.vert")).c_str());
	fragmentShader = App->program->CompileShader(GL_FRAGMENT_SHADER, (App->program->readFile("Shaders/skybox.frag")).c_str());
	skyboxProgram = App->program->CreateProgram(vertexSahder, fragmentShader);

	return true;

}

std::string ModuleProgram::readFile(const char* filePath) {

	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		LOG(_ERROR, "Could not read file %s. File does not exist.", filePath);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

unsigned int ModuleProgram::CompileShader(unsigned int type, const char* source) {

	GLuint shader_id = glCreateShader(type);
	glShaderSource(shader_id, 1, &source, 0);
	glCompileShader(shader_id);
	int res = GL_FALSE;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &res);

	if (res == GL_FALSE) {

		int len = 0;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);

		if (len > 0) {

			int written = 0;
			char* info = (char*)malloc(len);
			glGetShaderInfoLog(shader_id, len, &written, info);
			LOG(_ERROR, "Compile Shader  - Log Info: %s", info);
			free(info);
		}
	}
	return shader_id;
}

unsigned int ModuleProgram::CreateProgram(unsigned int vtx_shader, unsigned int frg_shader) {

	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vtx_shader);
	glAttachShader(program_id, frg_shader);
	glLinkProgram(program_id);
	int res;
	glGetProgramiv(program_id, GL_LINK_STATUS, &res);

	if (res == GL_FALSE) {

		int len = 0;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &len);
		if (len > 0) {

			int written = 0;
			char* info = (char*)malloc(len);
			glGetProgramInfoLog(program_id, len, &written, info);
			LOG(_ERROR, "Create Program - Program Log Info: %s", info);
			free(info);
		}
	}

	glDeleteShader(vtx_shader);
	glDeleteShader(frg_shader);

	return program_id;
}

bool ModuleProgram::CleanUp() {

	if (mainProgram != 0) glDeleteProgram(mainProgram);

	return true;
}