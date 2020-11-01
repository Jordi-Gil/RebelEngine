#include "Utils/Globals.h"
#include "Main/Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleEditorCamera.h"

#include <SDL/SDL.h>
#include <GL/glew.h>

ModuleRender::ModuleRender() {

}

// Destructor
ModuleRender::~ModuleRender() {

}

// Called before render is available
bool ModuleRender::Init() {

	LOG("Creating Renderer context");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // desired version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // we want a double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // we want to have a depth buffer with 24 bits
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // we want to have a stencil buffer with 8 bits

	mainContext = SDL_GL_CreateContext(App->window->window);

	if (mainContext == nullptr) {
		LOG("OpenGL context could not be created!SDL Error : % s", SDL_GetError());

		return false;
	}

	if (glewInit()) {
		LOG("Unable to initialize OpenGL!");
		return false;
	}

	LOG("Using Glew %s", glewGetString(GLEW_VERSION));
	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEPTH_TEST); // Enable depth test
	glEnable(GL_CULL_FACE); // Enable cull backward faces
	glFrontFace(GL_CCW); // Front faces will be counter clockwise

	return true;
}

update_status ModuleRender::PreUpdate() {

	int width, heigh;
	SDL_GetWindowSize(App->window->window, &width, &heigh);

	glViewport(0, 0, width, heigh);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (App->editorCamera->projectionChange()) {
		glMatrixMode(GL_PROJECTION);
		float4x4 projection;
		App->editorCamera->GetMatrix(PROJECTION_MATRIX, projection);
		glLoadMatrixf((float*)projection.v);
		App->editorCamera->setUpdated(false);
	}

	glMatrixMode(GL_MODELVIEW);
	float4x4 view;
	App->editorCamera->GetMatrix(VIEW_MATRIX, view);
	glLoadMatrixf((float*)view.v);

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update() {

#pragma region wireframe

	glLineWidth(1.0f);
	float d = 1500.0f;
	glBegin(GL_LINES);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}
	glEnd();

	glLineWidth(2.0f);
	glBegin(GL_LINES);
	// red X
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(2.0f, 0.0f, 0.0f);
	glVertex3f(2.0f, 0.1f, 0.0f); glVertex3f(2.1f, -0.1f, 0.0f);
	glVertex3f(2.1f, 0.1f, 0.0f); glVertex3f(2.0f, -0.1f, 0.0f);
	// green Y
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 2.0f, 0.0f);
	glVertex3f(-0.05f, 2.25f, 0.0f); glVertex3f(0.0f, 2.15f, 0.0f);
	glVertex3f(0.05f, 2.25f, 0.0f); glVertex3f(0.0f, 2.15f, 0.0f);
	glVertex3f(0.0f, 2.15f, 0.0f); glVertex3f(0.0f, 2.05f, 0.0f);

	// blue Z
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 2.0f);
	glVertex3f(-0.05f, 0.1f, 2.05f); glVertex3f(0.05f, 0.1f, 2.05f);
	glVertex3f(0.05f, 0.1f, 2.05f); glVertex3f(-0.05f, -0.1f, 2.05f);
	glVertex3f(-0.05f, -0.1f, 2.05f); glVertex3f(0.05f, -0.1f, 2.05f);
	glEnd();
	glLineWidth(1.0f);

#pragma endregion wireframe

#pragma region cube

	// Render a cube
	glBegin(GL_QUADS);
	// Top face
	glColor3f(0.8f, 0.8f, 0.8f);  // Green
	glVertex3f(1.0f, 1.0f, -1.0f);  // Top-right of top face
	glVertex3f(-1.0f, 1.0f, -1.0f);  // Top-left of top face
	glVertex3f(-1.0f, 1.0f, 1.0f);  // Bottom-left of top face
	glVertex3f(1.0f, 1.0f, 1.0f);  // Bottom-right of top face

	// Bottom face
	glVertex3f(1.0f, -1.0f, -1.0f); // Top-right of bottom face
	glVertex3f(-1.0f, -1.0f, -1.0f); // Top-left of bottom face
	glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom-left of bottom face
	glVertex3f(1.0f, -1.0f, 1.0f); // Bottom-right of bottom face

	// Front face
	glVertex3f(1.0f, 1.0f, 1.0f);  // Top-Right of front face
	glVertex3f(-1.0f, 1.0f, 1.0f);  // Top-left of front face
	glVertex3f(-1.0f, -1.0f, 1.0f);  // Bottom-left of front face
	glVertex3f(1.0f, -1.0f, 1.0f);  // Bottom-right of front face

	// Back face
	glVertex3f(1.0f, -1.0f, -1.0f); // Bottom-Left of back face
	glVertex3f(-1.0f, -1.0f, -1.0f); // Bottom-Right of back face
	glVertex3f(-1.0f, 1.0f, -1.0f); // Top-Right of back face
	glVertex3f(1.0f, 1.0f, -1.0f); // Top-Left of back face

	// Left face
	glVertex3f(-1.0f, 1.0f, 1.0f);  // Top-Right of left face
	glVertex3f(-1.0f, 1.0f, -1.0f);  // Top-Left of left face
	glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom-Left of left face
	glVertex3f(-1.0f, -1.0f, 1.0f);  // Bottom-Right of left face

	// Right face
	glVertex3f(1.0f, 1.0f, 1.0f);  // Top-Right of left face
	glVertex3f(1.0f, 1.0f, -1.0f);  // Top-Left of left face
	glVertex3f(1.0f, -1.0f, -1.0f);  // Bottom-Left of left face
	glVertex3f(1.0f, -1.0f, 1.0f);  // Bottom-Right of left face
	glEnd();
#pragma endregion cube

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate() {

	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp() {

	LOG("Destroying renderer");

	//Destroy window
	SDL_GL_DeleteContext(mainContext);

	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height) {

	glViewport(0, 0, width, height);
}