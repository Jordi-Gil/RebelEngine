#include "Utils/Globals.h"
#include "Main/Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"

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

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update() {

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

	LOG("Resizing the window");
	glViewport(0, 0, width, height);
}

