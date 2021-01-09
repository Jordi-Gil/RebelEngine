#pragma once
#include <windows.h>
#include <stdio.h>

#include <algorithm>

#define LOG(type, format,  ...) log(__FILE__, __LINE__, type, format, __VA_ARGS__);
#define MARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))

void log(const char file[], int line, const char* type, const char* format, ...);


typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define RESIZABLE true
#define FULLSCREEN false
#define FULLDESKTOP false
#define BORDERLESS false

#define VSYNC true
#define TITLE "Rebel Engine"
#define ORGANIZATION "Rebel Technologies"
#define VERSION "1.0.0"

#define _ERROR "[error]"
#define _INFO "[info]"
#define _WARNING "[warning]"

#define DEFAULT_SCENE_PATH "./Assets/Scenes/"
#define DEFAULT_SCENE_NAME "Scene_default"
#define DEFAULT_SCENE_EXT ".rebel"

#define DEFAULT_MESH_PATH "./Assets/Models/"
#define DEFAULT_MESH_EXT ".mesh"

#define JSON_TAG_ACTIVE "Active"
#define JSON_TAG_BACKGROUND_TYPE "Background_type"
#define JSON_TAG_COLOR "Color"
#define JSON_TAG_COMPONENTS "Components"
#define JSON_TAG_GAMEOBJECTS "GameObjects"
#define JSON_TAG_GLOBAL_MATRIX "GlobalMatrix"
#define JSON_TAG_LOCAL_MATRIX "LocalMatrix"
#define JSON_TAG_MATERIAL_INDEX "MaterialIndex"
#define JSON_TAG_MESH_PATH "MeshPath"
#define JSON_TAG_NAME "Name"
#define JSON_TAG_NORMALS "Normals"
#define JSON_TAG_NUM_INDICES "NumIndices"
#define JSON_TAG_NUM_FACES "NumFaces"
#define JSON_TAG_NUM_VERTICES "NumVertices"
#define JSON_TAG_OWNER "Owner"
#define JSON_TAG_POSITION "Position"
#define JSON_TAG_ROOT "Root"
#define JSON_TAG_ROTATION "Rotation"
#define JSON_TAG_ROTATION_QUAT "RotationQuat"
#define JSON_TAG_SCALE "Scale"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_UUID "UUID"
#define JSON_TAG_UVCOORDS "UVCoords"
#define JSON_TAG_VERTICES "Vertices"
#define JSON_TAG_ZFAR "ZFar"
#define JSON_TAG_ZNEAR "ZNear"





