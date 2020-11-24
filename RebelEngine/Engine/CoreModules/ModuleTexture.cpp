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

unsigned int ModuleTexture::loadTexture(const char* path, const char* meshPath) {

	unsigned int texId, image;

	ilGenImages(1, &texId); /* Generation of one image name */
	ilBindImage(texId); /* Binding of image name */
	bool success = ilLoadImage(path); /* Loading of image "image.jpg" */
	char filename[_MAX_FNAME], extension[_MAX_EXT];
	if (!success) {
		errno_t error = _splitpath_s(path, NULL, 0, NULL, 0, filename, _MAX_FNAME, extension, _MAX_EXT);
		if (error != 0) {
			LOG(_ERROR, "Couldn't split the given path. Error: ", strerror(error));
			return false;
		}
		else {
			char path2[500];
			sprintf(path2, "%s%s%s", meshPath, filename, extension);
			LOG(_INFO, "Trying to load image from %s", path2);
			success = ilLoadImage(path2);

			if (!success) {
				sprintf(path2, "Assets/Textures/%s%s", filename, extension);
				LOG(_INFO, "Trying to load image from %s", path2);
				success = ilLoadImage(path2);
			}
		}
	}

	if (success) {

		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); /* Convert every colour component into unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
		
		if (!success) {
			/* Error occured */
			LOG(_ERROR, "Cannot convet image");
			return false;
		}

		//unsigned char* data = ilGetData();

		glGenTextures(1, &image);
		glBindTexture(GL_TEXTURE_2D, image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

	}
	else
	{
		/* Error occured */
		return false;
	}

	ilDeleteImages(1, &texId);

	return image;

}