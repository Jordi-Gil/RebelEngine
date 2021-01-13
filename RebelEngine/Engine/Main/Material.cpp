#include "Material.h"

Material::Material(const std::string& name) {
	_name = name;
}

void Material::SetName(const std::string& name){
	_name = name;
}
