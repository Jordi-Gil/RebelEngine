#include "Material.h"

#include "Utils/RUUID.h"

#include "Utils/Globals.h"


Material::Material() {
	_uuid = RUUID::generate_uuid_v4();
}

Material::Material(const std::string& name) {
	_name = name;
	_uuid = RUUID::generate_uuid_v4();
}

void Material::SetName(const std::string& name){
	_name = name;
}

bool Material::FromJson(const Json::Value& value){

	if (!value.isNull()) {

		_name = _strdup(value[JSON_TAG_NAME].asCString());
		_uuid = value[JSON_TAG_UUID].asString();
		_type = material_type::NONE;

		return true;

	}
	else {
		return false;
	}

}

bool Material::WriteJsonFile() {

	_materialValue[0][JSON_TAG_NAME] = _name;
	_materialValue[0][JSON_TAG_UUID] = _uuid;
	_materialValue[0][JSON_TAG_TYPE] = _type;

	return true;
}