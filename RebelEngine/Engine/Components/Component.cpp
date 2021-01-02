#include "Component.h"

Component::Component(const Component& comp) {
	this->_owner = comp._owner;
	this->_active = comp._active;
	this->_type = comp._type;
}
