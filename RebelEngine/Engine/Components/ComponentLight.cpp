#include "ComponentLight.h"

#include "Math/TransformOps.h"
#include "Math/float3x3.h"

#include "ImGui/imgui_utils.h"
#include "ImGui/IconsFontAwesome5.h"
#include "ImGui/IconsForkAwesome.h"

#include "Main/GameObject.h"

#include "Utils/debugdraw.h"
#include <ImGui/imgui_internal.h>

static constexpr char* lightTypesLabel[3] = { "Directional Light", "Point Light", "Spot Light" };
static constexpr LightType lightTypes[3] = { LightType::kDIRECTIONAL_LIGHT, LightType::kPOINT_LIGHT, LightType::kSPOT_LIGHT };

ComponentLight::ComponentLight() {
	_type = ComponentType::kLIGHT;
}

ComponentLight::ComponentLight(LightType type) {

	_type = ComponentType::kLIGHT;
	_light_type = type;

}

ComponentLight::ComponentLight(const ComponentLight& copy) {

	_owner = copy._owner;
	_active = copy._active;
	_type = copy._type;
	_uuid = copy._uuid;

	_intensity = copy._intensity;
	_inner_angle = copy._inner_angle;
	_outter_angle = copy._outter_angle;
	_light_type = copy._light_type;
	
	_constant_att = copy._constant_att;
	_linear_att = copy._linear_att;
	_quadratic_att = copy._quadratic_att;
	_color = copy._color;

}

ComponentLight::ComponentLight(const Json::Value& value) {
	Component::FromJson(value);
	_type = ComponentType::kLIGHT;
	FromJson(value);
}

void ComponentLight::SetIntensity(float intensity) {
	_intensity = intensity;
}

void ComponentLight::SetInnerAngle(float inner_angle) {
	_inner_angle = inner_angle;
}

void ComponentLight::SetOutterAngle(float outter_angle) {
	_outter_angle = outter_angle;
}

void ComponentLight::SetColor(const float3& color) {
	_color = color;
}

void ComponentLight::ChangeType(LightType newType) {
	_light_type = newType;
}

bool ComponentLight::ToJson(Json::Value& value, int pos){
	
	Component::ToJson(value, pos);
	value[pos][JSON_TAG_LIGHT_INTENSITY] = _intensity;
	value[pos][JSON_TAG_LIGHT_INNER_ANGLE] = _inner_angle;
	value[pos][JSON_TAG_LIGHT_OUTTER_ANGLE] = _outter_angle;
	value[pos][JSON_TAG_LIGHT_TYPE] = (int)_light_type;
	value[pos][JSON_TAG_LIGHT_CONSTANT_ATT] = _constant_att;
	value[pos][JSON_TAG_LIGHT_LINEAR_ATT] = _linear_att;
	value[pos][JSON_TAG_LIGHT_QUADRATIC_ATT] = _quadratic_att;

	Json::Value arr(Json::arrayValue);
	arr.append(_color.x); arr.append(_color.y); arr.append(_color.z);
	value[pos][JSON_TAG_LIGHT_COLOR] = arr;

	return true;
}

bool ComponentLight::FromJson(const Json::Value& value){

	if (!value.isNull()) {

		_intensity = value[JSON_TAG_LIGHT_INTENSITY].asFloat();
		_inner_angle = value[JSON_TAG_LIGHT_INNER_ANGLE].asFloat();
		_outter_angle = value[JSON_TAG_LIGHT_OUTTER_ANGLE].asFloat();
		_constant_att = value[JSON_TAG_LIGHT_CONSTANT_ATT].asFloat();
		_linear_att = value[JSON_TAG_LIGHT_LINEAR_ATT].asFloat();
		_quadratic_att = value[JSON_TAG_LIGHT_QUADRATIC_ATT].asFloat();
		_light_type = (LightType) value[JSON_TAG_LIGHT_TYPE].asInt();
		_color = float3(
				value[JSON_TAG_LIGHT_COLOR][0].asFloat(),
				value[JSON_TAG_LIGHT_COLOR][1].asFloat(),
				value[JSON_TAG_LIGHT_COLOR][2].asFloat()
		);

		return true;
	}
	else return false;

}

void ComponentLight::DebugDraw() {

	if(_owner->IsSelected()){

		switch (_light_type) {

			case LightType::kSPOT_LIGHT: {
				float4x4 global = _owner->GetGlobalMatrix();
				float3 position = global.TranslatePart();
				float3 forward = global.WorldZ();

				dd::cone(position, forward, dd::colors::Yellow, DegToRad(_inner_angle), DegToRad(_outter_angle));
				break;
			}

			case LightType::kPOINT_LIGHT: {

				float3 position = _owner->GetGlobalMatrix().TranslatePart();
				dd::sphere(position, dd::colors::Yellow, 1);
				break;

			}

			case LightType::kDIRECTIONAL_LIGHT: {

				float4x4 global = _owner->GetGlobalMatrix();
				float3 position = global.TranslatePart();
				float3 forward = global.WorldZ();

				for (int i = 0; i < 10; i++) {

					float theta = (2.0f * pi * float(i)) / float(10);//get the current angle

					float3 p00 = position;
					float x = 0.15f * cosf(theta);//calculate the x component
					float y = 0.15f * sinf(theta);//calculate the y component
					p00.x += x; p00.y += y;
					float3 p01 = position + forward;
					p01.x += x; p01.y += y;

					dd::arrow(p00, p01, dd::colors::Yellow, 0.01f);

				}
			}
		}
	}
}

void ComponentLight::OnEditor() {

	static int current = (int) _light_type;

	if (ImGui::CollapsingHeader(ICON_FA_LIGHTBULB " Light", ImGuiTreeNodeFlags_DefaultOpen)) {

		std::string id;
		id = "##type_"; id.append(_uuid);
		ImGui::PushID(id.c_str());
		if (ImGui::Combo("Type", &current, lightTypesLabel, IM_ARRAYSIZE(lightTypesLabel))) {
			_light_type = lightTypes[current];
		};
		ImGui::PopID();

		id = "##color_"; id.append(_uuid);
		ImGui::PushID(id.c_str());
		ImGui::ColorEdit4("Color", &_color[0], ImGuiColorEditFlags_NoInputs);		
		ImGui::PopID();

		id = "##intensity_"; id.append(_uuid);
		ImGui::PushID(id.c_str());
		ImGui::DragFloat("Intensity", &_intensity, 0.01f, 0, 500, "%.2f");
		ImGui::PopID();

		if (_light_type == LightType::kPOINT_LIGHT || _light_type == LightType::kSPOT_LIGHT) {
			
			ImGui::NewLine();
			
			id = "Constant##constant_"; id.append(_uuid);
			ImGui::PushID(id.c_str());
			ImGui::Text("1");
			ImGui::PopID();

			id = "##linear_"; id.append(_uuid);
			ImGui::PushID(id.c_str());
			ImGui::DragFloat("Linear", &_linear_att, 0.001f, 0, 1, "%.2f");
			ImGui::PopID();

			id = "##constant_"; id.append(_uuid);
			ImGui::PushID(id.c_str());
			ImGui::DragFloat("Constant", &_quadratic_att, 0.001f, 0, 1, "%.2f");
			ImGui::PopID();

			if (_light_type == LightType::kSPOT_LIGHT) {
				
				id = "##inner_"; id.append(_uuid);
				ImGui::PushID(id.c_str());
				ImGui::DragFloat("Inner Angle", &_inner_angle, 0.1f, 1, 179, "%.2f");
				ImGui::PopID();

				id = "##outter_"; id.append(_uuid);
				ImGui::PushID(id.c_str());
				ImGui::DragFloat("Outter Angle", &_outter_angle, 0.1f, 1, 179, "%.2f");
				ImGui::PopID();

			}

		}


	}


}