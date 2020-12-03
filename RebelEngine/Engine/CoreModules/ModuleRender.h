#pragma once

#include "Module.h"
#include "Utils/Globals.h"

class ModuleRender : public Module {

public:

	ModuleRender() {}
	~ModuleRender() {}

	bool Init() override;
	bool Start() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	void Draw(float width, float height);
	void BindBuffers(float width, float height);

	void* GetContext() { return mainContext; }
	unsigned int GetViewportTexture() { return viewportTex; }

	void SetVSYNC(bool _VSYNC);
	void EnableAlphaTest(bool _alpha);
	void EnableBlend(bool _blend);
	void EnableCullFace(bool _cull);
	void EnableDepthTest(bool _depth);
	void EnableScissorTest(bool _scissor);
	void EnableStencilTest(bool _stencil);

public:

	float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

private:

	void* mainContext = nullptr;

	unsigned int FBO = 0;
	unsigned int RBO = 0;
	unsigned int viewportTex = 0;

};
