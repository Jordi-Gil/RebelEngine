#include "ModuleTexture.h"

#include "Utils/Console.h"

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

unsigned int ModuleTexture::loadTexture(const char* path) {

	unsigned int texId, image;

	ilGenImages(1, &texId); /* Generation of one image name */
	ilBindImage(texId); /* Binding of image name */
	bool success = ilLoadImage(path); /* Loading of image "image.jpg" */
	if (success) /* If no error occured: */
	{
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); /* Convert every colour component into unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
		if (!success)
		{
			/* Error occured */
			return false;
		}

		//unsigned char* data = ilGetData();

		glGenTextures(1, &image); /* Texture name generation */
		glBindTexture(GL_TEXTURE_2D, image); /* Binding of texture name */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear interpolation for magnification filter */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear interpolation for minifying filter */
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

	}
	else
	{
		/* Error occured */
		return false;
	}

	ilDeleteImages(1, &texId); /* Because we have already copied image data into texture data we can release memory used by image. */

	return image;

}