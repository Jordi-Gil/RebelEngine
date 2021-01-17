#pragma once
#include "Component.h"

#include "Math/float3.h"

enum light_type {
	DIRECTIONAL_LIGHT = 0,
	SPOT_LIGHT,
	POINT_LIGHT
};

class ComponentLight : public Component {

public:

	ComponentLight();
	ComponentLight(light_type type);
	ComponentLight(const ComponentLight& copy);
	ComponentLight(const Json::Value& value);

	void SetIntensity(float intesity);
	void SetInnerAngle(float inner_angle);
	void SetOutterAngle(float outter_angle);
	void SetColor(const float3& color);
	void ChangeType(light_type newType);

	float GetIntensity() const { return _intensity; };
	float GetInnerAngle() const { return _inner_angle; }
	float GetOutterAngle() const { return _outter_angle; }
	float GetConstantAtt() const { return _constant_att; }
	float GetLinearAtt() const { return _linear_att; }
	float GetQuadraticAtt() const { return _quadratic_att; }
	light_type GetLightType() const { return _light_type; }
	float3 GetColor() const { return _color; }

	void DebugDraw() override;
	void OnEditor() override;

	bool ToJson(Json::Value& value, int pos) override;
	bool FromJson(const Json::Value& value) override;

private:

	float _intensity = 1.0f;
	float _inner_angle = 1.0f;
	float _outter_angle = 20.0f;
	float _constant_att = 1.0f; //Always 1
	float _linear_att = 0.09;
	float _quadratic_att = 0.032;
	light_type _light_type;

	float3 _color = float3(1,1,1);
};

