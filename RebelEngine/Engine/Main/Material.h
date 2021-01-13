#pragma once

#include <string>

enum material_type {
	NONE,
	SPECULAR_STANDARD
};

class Material {

public:

	Material() : _name("New Material"), _type(material_type::NONE) {}
	Material(material_type type) : _type(type) {}
	Material(const std::string& name);

	void SetName(const std::string& name);

	const material_type GetType() const { return _type; }

protected:

	std::string _name;
	material_type _type;
};

