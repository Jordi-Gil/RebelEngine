#include "ModuleEditorCamera.h"
#include "ModuleInput.h"
#include "ModuleModel.h"

#include "Main/Application.h"

#include "GUIs/GUIScene.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"

#include <SDL/SDL.h>
#include <cmath>

ModuleEditorCamera::~ModuleEditorCamera() {

	delete camera;
	camera = nullptr;

}

bool ModuleEditorCamera::Init() {

	camera = new ComponentCamera();

	return true;
}

#pragma region transforms

void ModuleEditorCamera::TranslateKeyboard() {

	float3 movement = float3::zero;

	float speedModifier = 1.0f;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) speedModifier += 2;

	if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)	movement += camera->GetFront();
	if (App->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)	movement -= camera->GetRight();
	if (App->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)	movement -= camera->GetFront();
	if (App->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT) movement += camera->GetRight();
	if (App->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT) movement += float3::unitY;
	if (App->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT) movement -= float3::unitY;

	camera->Translate(movement * App->deltaTime * movSpeed * speedModifier);
}

void ModuleEditorCamera::TranslateMouse(int x, int y) {

	float3 movement = float3::zero;

	float speedModifier = 1.0f;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) speedModifier += 2;

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT &&
		App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT && x != 0 && y != 0) {

		movement.z -= (float)x;
		movement.z -= (float)y;
	}

	camera->Translate(movement * App->deltaTime * 0.5 * speedModifier);
}

void ModuleEditorCamera::TranslateMouseWheel() {

	float3 movement = float3::zero;

	float speedModifier = 1.0f;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) speedModifier *= 2;

	int dir = App->input->GetMouseWheel();

	if (dir != 0) {

		if (dir > 0)	movement += camera->GetFront() * 2;
		else movement -= camera->GetFront() * 2;
	}

	camera->Translate(movement * App->deltaTime * zoomSpeed * speedModifier);
}

void ModuleEditorCamera::RotateKeyboard() {

	float _yaw = 0.0f; float _pitch = 0.0f;
	float speedModifier = 1.0f;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT) _yaw += 1;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT) _yaw -= 1;
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT) _pitch -= 1;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT) _pitch += 1;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) speedModifier += 2;

	Quat quaternionX(camera->GetRight(), _yaw * App->deltaTime * rotSpeed * speedModifier);
	Quat quaternionY(float3::unitY, _pitch * App->deltaTime * rotSpeed * speedModifier);

	float3x3 rotationMatrixX = float3x3::FromQuat(quaternionX);
	float3x3 rotationMatrixY = float3x3::FromQuat(quaternionY);
	float3x3 rotationMatrix = rotationMatrixX * rotationMatrixY;

	vec oldFront = camera->GetFront().Normalized();
	camera->SetFront(rotationMatrix.MulDir(oldFront));
	vec oldUp = camera->GetUp().Normalized();
	camera->SetUp(rotationMatrix.MulDir(oldUp));

}

void ModuleEditorCamera::RotateMouse(int x, int y) {

	float _pitch = 0.0f; float _yaw = 0.0f;

	float speedModifier = 1.0f;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) speedModifier *= 2;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT &&
		App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_IDLE
		&& x != 0 && y != 0) {

		_pitch = -(float)y * App->deltaTime * rotSpeed;
		_yaw = -(float)x * App->deltaTime * rotSpeed;




		Quat quaternionX(camera->GetRight(), _pitch * speedModifier);
		Quat quaternionY(float3::unitY, _yaw * speedModifier);

		float3x3 rotationMatrixX = float3x3::FromQuat(quaternionX);
		float3x3 rotationMatrixY = float3x3::FromQuat(quaternionY);
		float3x3 rotationMatrix = rotationMatrixY * rotationMatrixX;

		vec oldFront = camera->GetFront().Normalized();
		camera->SetFront(rotationMatrix.MulDir(oldFront));
		vec oldUp = camera->GetUp().Normalized();
		camera->SetUp(rotationMatrix.MulDir(oldUp));
	}
}

void ModuleEditorCamera::OrbitCenterScene(int x, int y) {

	float _pitch = 0.0f; float _yaw = 0.0f;

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT &&
		App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT && x != 0 && y != 0
		) {

		_pitch = -(float)y * App->deltaTime * rotSpeed;
		_yaw = -(float)x * App->deltaTime * rotSpeed;

		this->pitch += _pitch;

		if (RadToDeg(pitch) > 85) { pitch = DegToRad(85); _pitch *= 0; }
		if (RadToDeg(pitch) < -85) { pitch = DegToRad(-85); _pitch *= 0; }

		vec focus = vec(0, 0, 0);//TODO: Get Center GameObject Clicked App->models->GetCenterScene();

		Quat rotationY(float3::unitY, _yaw);
		Quat rotationX(camera->GetRight().Normalized(), _pitch);

		vec newPos = rotationY.Transform(camera->GetPosition() - focus);
		newPos = rotationX.Transform(newPos);

		camera->SetPosition(newPos + focus);

		float3x3 rotationMatrix = float3x3::LookAt(
			camera->GetFront(),
			(focus - camera->GetPosition()).Normalized(),
			camera->GetUp(),
			float3::unitY
		);

		vec oldFront = camera->GetFront().Normalized();
		camera->SetFront(rotationMatrix.MulDir(oldFront));
		vec oldUp = camera->GetUp().Normalized();
		camera->SetUp(rotationMatrix.MulDir(oldUp));
	}

}

#pragma endregion transforms

update_status ModuleEditorCamera::Update() {

	if (App->gui->_scene->IsSceneFocused()) {
		int x, y;
		SDL_GetRelativeMouseState(&x, &y);
		TranslateKeyboard();
		TranslateMouse(x, y);
		TranslateMouseWheel();
		RotateKeyboard();
		RotateMouse(x, y);
		OrbitCenterScene(x, y);
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KeyState::KEY_DOWN) {
		Focus();
	}

	return UPDATE_CONTINUE;

}

void ModuleEditorCamera::Focus() {

	vec size = vec(1, 1, 1);//App->models->GetSizeScene();
	vec center = vec(0, 0, 0);//App->models->GetCenterScene();
	camera->SetPosition(center + camera->GetFront().Neg() * (size.Length() * 0.5f) * 2);
	camera->SetZFar(camera->GetZFar() * size.Length());
}

void ModuleEditorCamera::WindowResized(unsigned width, unsigned height) {

	if (width != 0 && height != 0) {
		float aspectRatio = float(width) / height;
		camera->SetVerticalFov(RadToDeg(camera->GetVerticalFov()), aspectRatio);
	}
}

void ModuleEditorCamera::GetMatrix(matrix_type _mType, float4x4& matrix) {

	switch (_mType) {
	case matrix_type::PROJECTION_MATRIX: {
		matrix = camera->GetProjectionMatrix();
		break;
	}
	case matrix_type::VIEW_MATRIX: {
		matrix = camera->GetViewMatrix();
		break;
	}
	}
}

void ModuleEditorCamera::GetOpenGLMatrix(matrix_type _mType, float4x4& matrix) {

	switch (_mType) {
	case matrix_type::PROJECTION_MATRIX: {
		matrix = camera->GetProjectionMatrix();
		break;
	}
	case matrix_type::VIEW_MATRIX: {
		matrix = camera->GetViewMatrix();
		break;
	}
	}

	matrix.Transpose();

}

#pragma region setters

void ModuleEditorCamera::SetVerticalFov(float vFov, float aspectRatio) {
	camera->SetVerticalFov(vFov, aspectRatio);
}

void ModuleEditorCamera::SetHorizontalFov(float hFov, float aspectRatio) {
	camera->SetHorizontalFov(hFov, aspectRatio);
}

void ModuleEditorCamera::SetPosition(float x, float y, float z) {
	camera->SetPosition(x, y, z);
}

void ModuleEditorCamera::SetZNear(float _znear) {
	camera->SetZNear(_znear);
}

void ModuleEditorCamera::SetZFar(float _zfar) {
	camera->SetZFar(_zfar);
}

void ModuleEditorCamera::SetMovSpeed(float _speed) {
	movSpeed = _speed;
}

void ModuleEditorCamera::SetRotSpeed(float _speed) {
	rotSpeed = _speed;
}

void ModuleEditorCamera::SetZoomSpeed(float _speed) {
	zoomSpeed = _speed;
}

#pragma endregion setters