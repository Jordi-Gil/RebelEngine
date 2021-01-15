#pragma once
#include "Component.h"

enum light_type {
	DIRECTIONAL_LIGHT,
	SPOT_LIGHT,
	POINT_LIGHT
	//AREA
};

class ComponentLight : public Component {

public:

	ComponentLight(light_type type, float intensity = 1.0f, float range = 10.0f, float angle = 30.0f) : _light_type(type), _intensity(intensity), _range(range), _spot_angle(angle) {}
	ComponentLight(const ComponentLight& copy);

	void SetIntensity(float intesity);
	void SetRange(float range);
	void SetSpotAngle(float angle);
	void ChangeType(light_type newType);

	float GetIntensity() const { return _intensity; };
	float GetRange() const { return _range; }
	float GetSpotAngle() const { return _spot_angle; }
	light_type GetLightType() const { return _light_type; }

	void DebugDraw() override;

private:

	float _intensity = 1.0f;
	float _range = 10.0f;
	float _spot_angle = 30.0f;
	light_type _light_type;
};

