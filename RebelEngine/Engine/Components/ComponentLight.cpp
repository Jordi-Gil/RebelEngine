#include "ComponentLight.h"

#include "Main/GameObject.h"

#include "Utils/debugdraw.h"

#include "Math/TransformOps.h"
#include "Math/float3x3.h"

void ComponentLight::DebugDraw() {
	
	switch (_light_type) {

		case SPOT_LIGHT: {
			float3x3 rot = (_owner->GetGlobalMatrix()).RotatePart();
			float3 position = (_owner->GetGlobalMatrix()).TranslatePart();
			float3 forward = rot.Col(2);

			dd::cone(position, -forward, dd::colors::Yellow, 1, 0.01);
			break;
		}

	}

}

ComponentLight::ComponentLight(const ComponentLight& copy) {

	_owner = copy._owner;
	_active = copy._active;
	_type = copy._type;
	_uuid = copy._uuid;

	_intensity = copy._intensity;
	_range = copy._range;
	_spot_angle = copy._spot_angle;
	_light_type = copy._light_type;

}

void ComponentLight::SetIntensity(float intensity) {
	_intensity = intensity;
}

void ComponentLight::SetRange(float range) {
	_range = range;
}

void ComponentLight::SetSpotAngle(float angle) {
	_spot_angle = angle;
}

void ComponentLight::ChangeType(light_type newType) {
	_light_type = newType;
}
