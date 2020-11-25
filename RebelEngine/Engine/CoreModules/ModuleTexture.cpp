#include "ModuleTexture.h"

#include <IL/il.h>
#include <GL/glew.h>

ModuleTexture::ModuleTexture() {}

bool ModuleTexture::Init() {

	LOG(_INFO, "Creating DevIL context");
	
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION) {
		/* wrong DevIL version */
		return false;
	}
	
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	return true;

}

unsigned int ModuleTexture::loadTexture(const char* path, const char* meshPath, TextureInformation &info) {

	unsigned int texId = 0, image;

	ilGenImages(1, &image);
	ilBindImage(image);

	bool success = ilLoadImage(path);
	
	char fileName[_MAX_FNAME], extension[_MAX_EXT];
	if (!success) {
		errno_t error = _splitpath_s(path, NULL, 0, NULL, 0, fileName, _MAX_FNAME, extension, _MAX_EXT);
		if (error != 0) {
			LOG(_ERROR, "Couldn't split the given path. Error: ", strerror(error));
			return false;
		}
		else {
			char path2[500];
			error = sprintf_s(path2, 500, "%s%s%s", meshPath, fileName, extension);
			if (error == -1) {
				LOG(_ERROR, "Error constructing path", path2);
				return false;
			}

			LOG(_INFO, "Trying to load image from %s", path2);
			success = ilLoadImage(path2);

			if (!success) {
				error = sprintf_s(path2, 500, "Assets/Textures/%s%s", fileName, extension);
				if (error == -1) {
					LOG(_ERROR, "Error constructing path", path2);
					return false;
				}
				LOG(_INFO, "Trying to load image from %s", path2);
				success = ilLoadImage(path2);

				if (!success) {
					LOG(_ERROR, "Texture not found.", path2);
					return false;
				}
				else info.name = path2;
			}
			else info.name = path2;
		}
	}
	else info.name = path;
	
	if(success) {
		
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

	}

	ilDeleteImages(1, &image);

	return texId;

}