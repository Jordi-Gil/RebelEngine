#pragma once

class Skybox {

public:

	Skybox();
	~Skybox();

	void Draw();

private:

	unsigned int skyTexture = 0;
	unsigned int VBO = 0;
	unsigned int VAO = 0;
};

