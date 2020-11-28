#include "ModuleEditorCamera.h"
#include "ModuleInput.h"
#include "ModuleModel.h"

#include "Main/Application.h"

#include "GUIs/GUIScene.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"

#include <SDL/SDL.h>
#include <cmath>

bool ModuleEditorCamera::Init() {

	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(currentZNear, currentZFar);
	frustum.SetHorizontalFovAndAspectRatio(DegToRad(90), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	frustum.SetPos(float3(0, 4, 10));
	frustum.SetFront(-float3::unitZ);
	frustum.SetUp(float3::unitY);

	return true;
}

#pragma region transforms

void ModuleEditorCamera::TranslateKeyboard() {

	float3 movement = float3::zero;

	float speedModifier = 1.0f;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) speedModifier += 2;

	if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)	movement += frustum.Front();
	if (App->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)	movement -= frustum.WorldRight();
	if (App->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)	movement -= frustum.Front();
	if (App->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT) movement += frustum.WorldRight();
	if (App->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_DOWN && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT) movement += float3::unitY;
	if (App->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_DOWN && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT) movement -= float3::unitY;

	frustum.Translate(movement * App->deltaTime * movSpeed * speedModifier);
}

void ModuleEditorCamera::TranslateMouse(int x, int y) {

	float3 movement = float3::zero;

	float speedModifier = 1.0f;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) speedModifier += 2;

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT && 
		App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT &&  x != 0 && y != 0) {

		movement.z -= (float)x;
		movement.z -= (float)y;
	}

	frustum.Translate(movement * App->deltaTime * 0.5 * speedModifier);
}

void ModuleEditorCamera::TranslateMouseWheel() {

	float3 movement = float3::zero;
	
	float speedModifier = 1.0f;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) speedModifier *= 2;

	int dir = App->input->GetMouseWheel();

	if (dir != 0) {

		if (dir > 0)	movement += frustum.Front() * 2;
		else movement -= frustum.Front() * 2;
	}

	frustum.Translate(movement * App->deltaTime * zoomSpeed * speedModifier);
}

void ModuleEditorCamera::RotateKeyboard() {

	float yaw = 0.0f; float pitch = 0.0f;
	float speedModifier = 1.0f;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT) yaw += 1;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT) yaw -= 1;
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT) pitch -= 1;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT) pitch += 1;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) speedModifier += 2;

	Quat quaternionX(frustum.WorldRight(), yaw * App->deltaTime * rotSpeed * speedModifier);
	Quat quaternionY(float3::unitY, pitch * App->deltaTime * rotSpeed * speedModifier);

	float3x3 rotationMatrixX = float3x3::FromQuat(quaternionX);
	float3x3 rotationMatrixY = float3x3::FromQuat(quaternionY);
	float3x3 rotationMatrix = rotationMatrixX * rotationMatrixY;

	vec oldFront = frustum.Front().Normalized();
	frustum.SetFront(rotationMatrix.MulDir(oldFront));
	vec oldUp = frustum.Up().Normalized();
	frustum.SetUp(rotationMatrix.MulDir(oldUp));

}

void ModuleEditorCamera::RotateMouse(int x, int y) {

	float pitch = 0.0f; float yaw = 0.0f;

	float speedModifier = 1.0f;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) speedModifier *= 2;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT &&
		App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_IDLE
		&& x != 0 && y != 0) {

		pitch = -(float)y * App->deltaTime * rotSpeed;
		yaw = -(float)x * App->deltaTime * rotSpeed;




		Quat quaternionX(frustum.WorldRight(), pitch * speedModifier);
		Quat quaternionY(float3::unitY, yaw * speedModifier);

		float3x3 rotationMatrixX = float3x3::FromQuat(quaternionX);
		float3x3 rotationMatrixY = float3x3::FromQuat(quaternionY);
		float3x3 rotationMatrix = rotationMatrixY * rotationMatrixX;

		vec oldFront = frustum.Front().Normalized();
		frustum.SetFront(rotationMatrix.MulDir(oldFront));
		vec oldUp = frustum.Up().Normalized();
		frustum.SetUp(rotationMatrix.MulDir(oldUp));
	}
}

void ModuleEditorCamera::OrbitCenterScene(int x, int y) {

	float pitch = 0.0f; float yaw = 0.0f;

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT &&
		App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT && x != 0 && y != 0
		) {

		pitch = -(float)y * App->deltaTime * rotSpeed;
		yaw = -(float)x * App->deltaTime * rotSpeed;

		this->pitch += pitch;

		if (RadToDeg(this->pitch) > 85) { 
			this->pitch = DegToRad(85);
			pitch *= 0; 
		}
		if (RadToDeg(this->pitch) < -85) { this->pitch = DegToRad(-85); pitch *= 0; }

		vec focus = App->models->GetCenterScene();

		Quat rotationY(float3::unitY, yaw);
		Quat rotationX(frustum.WorldRight().Normalized(), pitch);

		vec newPos = rotationY.Transform(frustum.Pos() - focus); //perfom the rotation over Up vector in the origin
		newPos = rotationX.Transform(newPos); //perfom the rotation over Right vector in the origin

		frustum.SetPos(newPos + focus); //Once the rotation is performed, move the camera at focus distance
	
		//Make frustum points the target
		float3x3 rotationMatrix = float3x3::LookAt(
			frustum.Front(),
			(focus - frustum.Pos()).Normalized(),
			frustum.Up(),
			float3::unitY
		);
		
		vec oldFront = frustum.Front().Normalized();
		frustum.SetFront(rotationMatrix.MulDir(oldFront));
		vec oldUp = frustum.Up().Normalized();
		frustum.SetUp(rotationMatrix.MulDir(oldUp));
	}

}

#pragma endregion transforms

update_status ModuleEditorCamera::Update() {

	if (App->gui->scene->IsSceneFocused()) {
		int x, y;
		SDL_GetRelativeMouseState(&x, &y);
		TranslateKeyboard();
		TranslateMouse(x, y);
		TranslateMouseWheel();
		RotateKeyboard();
		RotateMouse(x, y);
		OrbitCenterScene(x,y);
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KeyState::KEY_DOWN) {
		Focus();
	}

	return UPDATE_CONTINUE;

}

void ModuleEditorCamera::Focus() {

	vec size = App->models->GetSizeScene();
	vec center = App->models->GetCenterScene();

	frustum.SetPos(center + frustum.Front().Neg() * size.Length() * 1.5 );

	vec target = (center - frustum.Pos()).Normalized();
	float3x3 rotationMatrix = float3x3::LookAt(frustum.Front(), target, frustum.Up(), float3::unitY);

	vec oldFront = frustum.Front();
	frustum.SetFront(rotationMatrix.MulDir(oldFront));
	vec oldUp = frustum.Up();
	frustum.SetUp(rotationMatrix.MulDir(oldUp));

	frustum.SetViewPlaneDistances(frustum.NearPlaneDistance(), frustum.FarPlaneDistance() * size.Length());
}

void ModuleEditorCamera::WindowResized(unsigned width, unsigned height) {

	if (width != 0 && height != 0) {
		float aspectRatio = float(width) / height;
		frustum.SetVerticalFovAndAspectRatio(frustum.VerticalFov(), aspectRatio);
	}
}

void ModuleEditorCamera::GetMatrix(matrix_type _mType, float4x4& matrix) {

	switch (_mType) {
		case matrix_type::PROJECTION_MATRIX: {
			matrix = frustum.ProjectionMatrix();
			break;
		}
		case matrix_type::VIEW_MATRIX: {
			matrix = frustum.ViewMatrix();
			break;
		}
	}
}

void ModuleEditorCamera::GetOpenGLMatrix(matrix_type _mType, float4x4& matrix) {

	switch (_mType) {
		case matrix_type::PROJECTION_MATRIX: {
			matrix = frustum.ProjectionMatrix();
			break;
		}
		case matrix_type::VIEW_MATRIX: {
			matrix = frustum.ViewMatrix();
			break;
		}
	}

	matrix.Transpose();

}

#pragma region setters

void ModuleEditorCamera::SetVerticalFov(float vFov, float aspectRatio) {
	frustum.SetVerticalFovAndAspectRatio(DegToRad(vFov), aspectRatio);
}

void ModuleEditorCamera::SetHorizontalFov(float hFov, float aspectRatio) {
	frustum.SetHorizontalFovAndAspectRatio(DegToRad(hFov), aspectRatio);
}

void ModuleEditorCamera::SetPosition(float x, float y, float z) {
	frustum.SetPos(vec(x,y,z));
}

void ModuleEditorCamera::SetZNear(float _znear) {
	currentZNear = _znear;
	frustum.SetViewPlaneDistances(currentZNear, currentZFar);
}

void ModuleEditorCamera::SetZFar(float _zfar) {
	currentZFar = _zfar;
	frustum.SetViewPlaneDistances(currentZNear, currentZFar);
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