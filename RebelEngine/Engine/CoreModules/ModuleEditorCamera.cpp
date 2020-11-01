#include "ModuleEditorCamera.h"
#include "ModuleInput.h"
#include "Main/Application.h"
#include "Math/float3x3.h"
#include "Math/Quat.h"
#include <SDL/SDL.h>
#include <cmath>

void ModuleEditorCamera::GetMatrix(matrix_type _mType, float4x4& matrix) {

	switch (_mType) {
		case PROJECTION_MATRIX:
		{
			matrix = frustum.ProjectionMatrix().Transposed();
			break;
		}
		case VIEW_MATRIX:
		{
			matrix = frustum.ViewMatrix();
			matrix.Transpose();
		}
	}
}

#pragma region transforms

void ModuleEditorCamera::TranslateKeyboard() {

	float3 movement = float3::zero;

	if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT)	movement += frustum.Front();
	if (App->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT)	movement -= frustum.WorldRight();
	if (App->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT)	movement -= frustum.Front();
	if (App->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT) movement += frustum.WorldRight();

	frustum.Translate(movement * App->deltaTime * movSpeed);

}

void ModuleEditorCamera::RotateKeyboard() {

	float roll = 0.0f; float pitch = 0.0f; float yaw = 0.0f;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT)
		roll += 1;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT)
		roll -= 1;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT)
		pitch -= 1;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT)
		pitch += 1;

	Quat quaternionX(frustum.WorldRight(), roll * App->deltaTime * rotSpeed);
	Quat quaternionY(float3::unitY, pitch * App->deltaTime * rotSpeed);

	float3x3 rotationMatrixX = float3x3::FromQuat(quaternionX);
	float3x3 rotationMatrixY = float3x3::FromQuat(quaternionY);
	float3x3 rotationMatrix = rotationMatrixX * rotationMatrixY;

	vec oldFront = frustum.Front().Normalized();
	frustum.SetFront(rotationMatrix.MulDir(oldFront));
	vec oldUp = frustum.Up().Normalized();
	frustum.SetUp(rotationMatrix.MulDir(oldUp));

}

#pragma endregion transforms

update_status ModuleEditorCamera::Update() {

	TranslateKeyboard();
	RotateKeyboard();

	return UPDATE_CONTINUE;

}

bool ModuleEditorCamera::Init() {

	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 200.0f);
	frustum.SetHorizontalFovAndAspectRatio((float)DEGTORAD * 90.0f, 1.3f);
	frustum.SetPos(float3(0, 5, -10));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	return true;
}

void ModuleEditorCamera::WindowResized(unsigned width, unsigned height) {

	if (width != 0 && height != 0) {
		float aspectRatio = float(width) / height;
		frustum.SetVerticalFovAndAspectRatio(frustum.VerticalFov(), aspectRatio);
	}

	updated = true;

}