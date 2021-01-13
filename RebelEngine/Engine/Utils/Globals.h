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

constexpr char* imageFormat[63] = {
	".blp", ".bmp", ".bw", ".cur", ".cut", ".dcm", ".dcx", ".dds", ".dicom", ".dpx", ".exr", ".fit",
	".fits", ".ftx", ".gif", ".h", ".hdp", ".hdr", ".icns", ".ico", ".iff", ".im", ".iwi", ".jng", ".jp2",
	".jpe", ".jpeg", ".jpg", ".lbm", ".lif", ".lmp", ".mdl", ".mng", ".mp3", ".pbm", ".pcd", ".pcx", ".pgm",
	".pic", ".pix", ".png", ".pnm", ".ppm", ".psd", ".psp", ".pxr", ".ras", ".rgb", ".rgba", ".rot", ".rs", ".sgi",
	".sun", ".texture", ".tga", ".tif", ".tiff", ".tpl", ".utx", ".vtf", ".wal", ".wdp", ".xpm" };

constexpr char* meshFormat[48] = { ".3d", ".3ds", ".ac", ".ase", ".b3d", ".blend", ".bvh", ".cob", ".csm",
".dae", ".dxf", ".fbx", ".glb", ".gltf", ".hmp", ".ifc", ".irr", ".irrmesh",
".lwo", ".lws", ".lxo", ".md2", ".md3", ".md5", ".mdc", ".mdl", ".mdl", ".ms3d",
".ndo", ".nff", ".nff", ".obj", ".off", ".ogex", ".pk3", ".ply", ".q3d", ".q3s",
".raw", ".scn", ".smd", ".stl", ".ter", ".vta", ".x", ".xgl", ".xml", ".zgl" };

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
#define JSON_TAG_ASPECT_RATIO "AspectRatio"
#define JSON_TAG_BACKGROUND_TYPE "Background_type"
#define JSON_TAG_COLOR "Color"
#define JSON_TAG_COMPONENTS "Components"
#define JSON_TAG_EBO "EBO"
#define JSON_TAG_EDITOR_CAMERA "EditorCamera"
#define JSON_TAG_FOV_HORIZONTAL "FOVHorizontal"
#define JSON_TAG_FRONT "Front"
#define JSON_TAG_GAMEOBJECTS "GameObjects"
#define JSON_TAG_GLOBAL_MATRIX "GlobalMatrix"
#define JSON_TAG_LOCAL_MATRIX "LocalMatrix"
#define JSON_TAG_MATERIAL_INDEX "MaterialIndex"
#define JSON_TAG_MESH_PATH "MeshPath"
#define JSON_TAG_NAME "Name"
#define JSON_TAG_NORMALS "Normals"
#define JSON_TAG_NUM_FACES "NumFaces"
#define JSON_TAG_NUM_INDICES "NumIndices"
#define JSON_TAG_NUM_VERTICES "NumVertices"
#define JSON_TAG_OWNER "Owner"
#define JSON_TAG_POSITION "Position"
#define JSON_TAG_ROOT "Root"
#define JSON_TAG_ROTATION "Rotation"
#define JSON_TAG_ROTATION_QUAT "RotationQuat"
#define JSON_TAG_SCALE "Scale"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_UP "UP"
#define JSON_TAG_UUID "UUID"
#define JSON_TAG_UVCOORDS "UVCoords"
#define JSON_TAG_VBO "VBO"
#define JSON_TAG_VBO_MAX "VBOMax"
#define JSON_TAG_VBO_MIN "VBOMin"
#define JSON_TAG_VBO_SIZE "VBOSize"
#define JSON_TAG_VERTICES "Vertices"
#define JSON_TAG_ZFAR "ZFar"
#define JSON_TAG_ZNEAR "ZNear"
