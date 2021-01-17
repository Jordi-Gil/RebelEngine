#pragma once

#include <string>

#include "json/json.h"

enum material_type {
	NONE,
	SPECULAR_STANDARD
};

class Material {

public:

	Material();
	Material(const std::string& name);
	Material(const Json::Value& value);

	void SetName(const std::string& name);
	void SetFilePath(const char* filePath) { sprintf_s(_filePath, "%s", filePath); }

	const material_type GetType() const { return _type; }
	const char* GetName() const { return _name.c_str(); }
	const char* GetFilePath() { return _filePath; }
	const char* GetUUID() const { return _uuid.c_str(); }

	bool FromJson(const Json::Value& value);
	bool WriteJsonFile();

protected:

	material_type _type = material_type::NONE;
	std::string _name = "New Material";
	std::string _uuid;
	char _filePath[1024];
	
	Json::Value _materialValue;
};

