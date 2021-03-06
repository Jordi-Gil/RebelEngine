#include "Component.h"
#include "Utils/RUUID.h"
#include "Main/GameObject.h"

Component::Component() {
	_uuid = RUUID::generate_uuid_v4();
}

Component::Component(const Component& comp) {
	_owner = comp._owner;
	_active = comp._active;
	_type = comp._type;
	_uuid = comp._uuid;
}

Component::Component(const Json::Value& value) {
	FromJson(value);
}

bool Component::ToJson(Json::Value& value, int pos) {

	value[pos][JSON_TAG_UUID] = _uuid;
	(_owner != nullptr) ? value[pos][JSON_TAG_OWNER] = _owner->GetUUID() : value[pos][JSON_TAG_OWNER] = "";
	value[pos][JSON_TAG_ACTIVE] = _active;
	value[pos][JSON_TAG_TYPE] = (int)_type;

	return true;
}

bool Component::FromJson(const Json::Value& value) 
{
	if (!value.isNull()) {
		_active = value[JSON_TAG_ACTIVE].asBool();
		_type = ComponentType::kNONE;
		_uuid = value[JSON_TAG_UUID].asCString();
	}
	else {
	//TODO: JSON ERROR
	}
	return true;
}
