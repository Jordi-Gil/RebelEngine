#include "Component.h"
#include "Utils/RUUID.h"
#include "Main/GameObject.h"

Component::Component() {
	_uuid = RUUID::generate_uuid_v4();
}

Component::Component(const Component& comp) {
	this->_owner = comp._owner;
	this->_active = comp._active;
	this->_type = comp._type;
	this->_uuid = comp._uuid;
}

bool Component::ToJson(Json::Value& value, int pos) {

	value[pos]["UUID"] = _uuid;
	(_owner != nullptr) ? value[pos]["OWNER"] = _owner->GetUUID() : value[pos]["OWNER"] = "";
	value[pos]["Active"] = _active;
	value[pos]["Type"] = (int)_type;

	return true;
}
