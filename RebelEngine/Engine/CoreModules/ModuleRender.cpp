#include "Main/Application.h"

#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleProgram.h"
#include "ModuleEditorCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleModel.h"

#include <SDL/SDL.h>
#include <GL/glew.h>

ModuleRender::ModuleRender() {

}

// Destructor
ModuleRender::~ModuleRender() {

}

void __stdcall OurOpenGLErrorFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {

	const char* tmp_source = "", * tmp_type = "", * tmp_severity = "", * type_msg = _WARNING;
	switch (source) {
	case GL_DEBUG_SOURCE_API: tmp_source = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: tmp_source = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: tmp_source = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: tmp_source = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: tmp_source = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: tmp_source = "Other"; break;
	};
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: tmp_type = "Error"; type_msg = _ERROR; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: tmp_type = "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: tmp_type = "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY: tmp_type = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE: tmp_type = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER: tmp_type = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP: tmp_type = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP: tmp_type = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER: tmp_type = "Other"; break;
	};
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: tmp_severity = "high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: tmp_severity = "medium"; break;
	case GL_DEBUG_SEVERITY_LOW: tmp_severity = "low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: tmp_severity = "notification"; break;
	};
	
	if (App->logTimer.getDeltaTime() >= 500) {
		LOG(type_msg, "[OpenGL Debug] <Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>", tmp_source, tmp_type, tmp_severity, id, message);
		App->logTimer.start();
	}
}

// Called before render is available
bool ModuleRender::Init() {

	LOG(_INFO, "Creating Renderer context");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // desired version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // we want a double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // we want to have a depth buffer with 24 bits
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // we want to have a stencil buffer with 8 bits

	mainContext = SDL_GL_CreateContext(App->window->window);

	if (mainContext == nullptr) {
		LOG(_INFO, "OpenGL context could not be created!SDL Error : % s", SDL_GetError());
		return false;
	}

	if (glewInit()) {
		LOG(_INFO, "Unable to initialize OpenGL!");
		return false;
	}

	LOG(_INFO, "Using Glew %s", glewGetString(GLEW_VERSION));
	LOG(_INFO, "Vendor: %s", glGetString(GL_VENDOR));
	LOG(_INFO, "Renderer: %s", glGetString(GL_RENDERER));
	LOG(_INFO, "OpenGL version supported %s", glGetString(GL_VERSION));
	LOG(_INFO, "GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEPTH_TEST); // Enable depth test
	glEnable(GL_CULL_FACE); // Enable cull backward faces
	glFrontFace(GL_CCW); // Front faces will be counter clockwise

#ifdef  _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
#endif //  _DEBUG

	return true;
}

bool ModuleRender::Start() {

	glGenFramebuffers(1, &FBO);

	return true;

}

void ModuleRender::BindBuffers(float width, float height) {

	if (viewportTex != 0) glDeleteTextures(1, &viewportTex);
	if (RBO != 0) glDeleteRenderbuffers(1, &RBO);

	//Re-Genereate the render buffer
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	//Generate the texture
	glGenTextures(1, &viewportTex);
	glBindTexture(GL_TEXTURE_2D, viewportTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (unsigned)width, (unsigned)height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, viewportTex, 0);

}

void ModuleRender::Draw(float width, float height) {

	//Bind the Frame Buffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	BindBuffers(width, height);

	GLenum  error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (error != GL_FRAMEBUFFER_COMPLETE)
		LOG(_ERROR, "RENDER ERROR, %d", error);

	glViewport(0, 0, width, height);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float4x4 projection; App->editorCamera->GetMatrix(PROJECTION_MATRIX, projection);
	float4x4 view; App->editorCamera->GetMatrix(VIEW_MATRIX, view);

	App->models->Draw();
	App->debugDraw->Draw(view, projection, width, height);

	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

update_status ModuleRender::PostUpdate() {

	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}
// Called before quitting
bool ModuleRender::CleanUp() {

	LOG(_INFO, "Destroying renderer");

	if (FBO != 0) glDeleteFramebuffers(1, &FBO);
	if (RBO != 0) glDeleteFramebuffers(1, &RBO);
	if (viewportTex != 0) glDeleteTextures(1, &viewportTex);

	//Destroy window
	SDL_GL_DeleteContext(mainContext);

	return true;
}