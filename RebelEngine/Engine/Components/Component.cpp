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

	value[pos][JSON_TAG_UUID] = _uuid;
	(_owner != nullptr) ? value[pos][JSON_TAG_OWNER] = _owner->GetUUID() : value[pos][JSON_TAG_OWNER] = "";
	value[pos][JSON_TAG_ACTIVE] = _active;
	value[pos][JSON_TAG_TYPE] = (int)_type;

	return true;
}
