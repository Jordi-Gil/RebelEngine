#include "ModuleTexture.h"

#include <GL/glew.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <vector>

GLenum minification[] = {
	GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_LINEAR
};

GLenum magnification[] = { GL_NEAREST, GL_LINEAR };

GLenum wrap[] = { GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT, GL_MIRROR_CLAMP_TO_EDGE };

bool ModuleTexture::Init() {

	LOG(_INFO, "Creating DevIL context");

	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION) {
		LOG(_ERROR, "Wrong DevIL version");
		return false;
	}

	ilInit();
	iluInit();
	ilutInit();

	return true;
}

unsigned int ModuleTexture::loadTexture(const char* path, TextureInformation& info) {

	unsigned int texId = 0, image;

	ilGenImages(1, &image);
	ilBindImage(image);

	bool success = ilLoadImage(path);

	ILenum error = ilGetError();

	if (error != IL_NO_ERROR) {
		LOG(_ERROR, "Error loading image %s, iluError [%d]: %s", path, error, iluErrorString(error));
	}

	if (success) {

		ILinfo ilImageInfo;
		iluGetImageInfo(&ilImageInfo);

		if (ilImageInfo.Origin == IL_ORIGIN_UPPER_LEFT) {
			iluFlipImage();
		}

		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		if (!success) {
			LOG(_ERROR, "Cannot every colour component into unsigned byte");
			return false;
		}

		info.w = ilGetInteger(IL_IMAGE_WIDTH);
		info.h = ilGetInteger(IL_IMAGE_HEIGHT);

		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
		glGenerateMipmap(GL_TEXTURE_2D);

	}

	ilDeleteImages(1, &image);

	return texId;

}

unsigned int ModuleTexture::loadCubeMap(const char* path)
{
	LOG(_INFO, "LOADING CUBEMAP");

	unsigned int texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

	std::vector<char*> faces{ "right.png","left.png",
	"top.png","bottom.png","front.png","back.png" };

	for (unsigned int i = 0; i < faces.size(); i++) {
		char realPath[_MAX_PATH];
		sprintf_s(realPath, _MAX_PATH, "%s%s", path, faces[i]);
		unsigned int image;
		ilGenImages(1, &image);
		ilBindImage(image);

		bool success = ilLoadImage(realPath);

		if (success) {

			success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

			if (!success) {
				LOG(_ERROR, "Cannot every colour component into unsigned byte");
				return false;
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, ilGetInteger(IL_IMAGE_BPP),
				ilGetInteger(IL_IMAGE_WIDTH),
				ilGetInteger(IL_IMAGE_HEIGHT), 0,
				ilGetInteger(IL_IMAGE_FORMAT),
				GL_UNSIGNED_BYTE,
				ilGetData());

		}

		ilDeleteImages(1, &image);

	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	LOG(_INFO, "CUBEMAP FINISHED");

	return texId;
}

void ModuleTexture::SetMinFilter(unsigned i, unsigned texID) {
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minification[i]);
}

void ModuleTexture::SetMagFilter(unsigned i, unsigned texID) {
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magnification[i]);
}

void ModuleTexture::SetWrapS(unsigned i, unsigned texID) {
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, i);
}

void ModuleTexture::SetWrapT(unsigned i, unsigned texID) {
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, i);
}

void ModuleTexture::SetWrapR(unsigned i, unsigned texID) {
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, i);
}

bool ModuleTexture::CleanUp() {
	ilShutDown();
	//No shutdown for ilu and ilut?

	return true;
}